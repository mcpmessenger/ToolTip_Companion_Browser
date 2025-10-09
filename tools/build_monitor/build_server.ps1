param(
  [string]$LogPath = "${PSScriptRoot}\..\..\src\src\out\Default\build_chrome.log",
  [int]$Port = 8787
)

$ErrorActionPreference = 'SilentlyContinue'

Add-Type -AssemblyName System.Net.HttpListener
$listener = [System.Net.HttpListener]::new()
$prefix = "http://localhost:$Port/"
$listener.Prefixes.Add($prefix)
$listener.Start()
Write-Host "Build monitor server listening at $prefix"

function Write-Response([System.Net.HttpListenerResponse]$resp, [string]$content, [string]$ct='text/html'){ $bytes = [System.Text.Encoding]::UTF8.GetBytes($content); $resp.ContentType=$ct; $resp.ContentLength64=$bytes.Length; $resp.OutputStream.Write($bytes,0,$bytes.Length); $resp.OutputStream.Close() }

$logQueue = [System.Collections.Concurrent.ConcurrentQueue[string]]::new()

$fsw = Get-Content -Path $LogPath -Wait -Tail 0 -ErrorAction SilentlyContinue | ForEach-Object { $logQueue.Enqueue($_) }

while ($listener.IsListening) {
  $ctx = $listener.GetContext()
  $req = $ctx.Request
  $res = $ctx.Response
  switch -Regex ($req.Url.AbsolutePath) {
    '^/$' {
      $html = Get-Content -LiteralPath "${PSScriptRoot}\build_live.html" -Raw
      Write-Response $res $html 'text/html; charset=utf-8'
    }
    '^/events$' {
      $res.ContentType = 'text/event-stream'
      $res.SendChunked = $true
      $res.Headers.Add('Cache-Control','no-cache')
      $out = $res.OutputStream
      $enc = [System.Text.Encoding]::UTF8
      $flush = { try { $res.OutputStream.Flush() } catch {} }
      $send = {
        param([string]$line)
        $data = "data: $line`n`n"
        $bytes = $enc.GetBytes($data)
        try { $out.Write($bytes,0,$bytes.Length) } catch {}
      }
      # initial burst
      $initial = Get-Content -Path $LogPath -Tail 200 -ErrorAction SilentlyContinue
      foreach ($l in $initial) { & $send $l }
      & $flush
      # live stream loop
      while ($res.OutputStream.CanWrite) {
        $drained = $false
        while ($true) {
          $ok,$line = $logQueue.TryDequeue([ref]$null)
          if (-not $ok) { break }
          & $send $line
          $drained = $true
        }
        if ($drained) { & $flush }
        Start-Sleep -Milliseconds 200
        if ($req.IsAuthenticated -eq $null) { break }
      }
      try { $res.OutputStream.Close() } catch {}
    }
    default {
      $res.StatusCode = 404
      Write-Response $res "404"
    }
  }
}



