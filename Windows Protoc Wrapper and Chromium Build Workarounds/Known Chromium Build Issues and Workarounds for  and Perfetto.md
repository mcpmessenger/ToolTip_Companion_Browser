## Known Chromium Build Issues and Workarounds for `protoc-gen-ts_proto` and Perfetto

The `protoc-gen-ts_proto` plugin, particularly in the context of Chromium builds on Windows, can encounter various issues, including the persistent null byte corruption in its batch file. This section compiles information from Chromium bug trackers, developer forums, and GitHub issues to identify known problems and potential workarounds.

### 1. `protoc-gen-ts_proto.bat` Null Byte Corruption and Encoding Issues

As detailed in `windows_batch_file_encoding_issues.md`, the `SyntaxError: source code cannot contain null bytes` is a common problem on Windows when batch files are saved with UTF-16LE encoding. This encoding introduces null bytes that the `cmd.exe` interpreter misinterprets as an End-Of-File marker, leading to truncated execution [1].

**Workarounds for Encoding:**

*   **Explicitly Save as ANSI or UTF-8 without BOM:** The most reliable solution is to ensure that any process generating `protoc-gen-ts_proto.bat` (or similar batch files) explicitly saves it with ANSI or UTF-8 without a Byte Order Mark (BOM). This might involve modifying Python or PowerShell scripts that create these files to specify the encoding [1].
*   **Convert Existing Files:** If the batch file is already corrupted, it can be converted using command-line tools or text editors to change its encoding to ANSI or UTF-8 without BOM [1].

### 2. Perfetto Protobuf Generation Issues

Chromium issue 406928883 highlights problems with Perfetto protobuf generation, where `protoc` invocations might produce output that, while not failing the build, indicates underlying issues. This suggests that Perfetto's TypeScript generation could be a source of `protoc`-related instability [2].

**Key Observations from Perfetto Issues:**

*   **`--fatal-warnings` Flag:** The `protoc` tool supports a `--fatal-warnings` flag, which can be used to treat warnings as errors, potentially making `protoc` failures more explicit [2].
*   **`third_party/protobuf/proto_library.gni`:** This file is central to how `protoc` arguments are passed for protobuf rules within Chromium. Modifications here could influence Perfetto's protobuf generation [2].

**Potential Workarounds for Perfetto-related `protoc` Issues:**

*   **Disable Perfetto Tracing (if possible):** While a direct `gn arg` to disable `traces_internals` or Perfetto TypeScript generation entirely is not immediately apparent, investigating `gn args` related to Perfetto (e.g., `enable_perfetto_tracing=false` or similar flags if they exist) could be a path forward. This would require a deeper dive into the Perfetto build configurations within the Chromium source tree.
*   **Target Specific Components:** If the `protoc-gen-ts_proto.bat` issue is specifically linked to Perfetto's TypeScript generation, building only the necessary components for `ToolTip_Companion_Browser` (as discussed in `chromium_individual_components_build.md`) might bypass the problematic Perfetto generation altogether [3].

### 3. General `protoc` Build Failures and Toolchain Issues

Other Chromium issues, such as 40513764, describe `protoc.exe` crashes and non-zero exit statuses, often linked to high parallelism, memory corruption, or toolchain incompatibilities [4].

**Workarounds for General `protoc` Issues:**

*   **`DEPOT_TOOLS_WIN_TOOLCHAIN=0`:** Setting this environment variable forces the Chromium build to use a manually installed Visual Studio and Windows SDK instead of the `depot_tools`-provided toolchain. This can resolve `protoc` issues stemming from toolchain incompatibilities or corruption [5].
*   **Reduce Parallelism:** Building with fewer parallel jobs (`-j` flag) can mitigate race conditions and dependency problems that sometimes lead to `protoc` failures [4].

### 4. `protoc-gen-ts_proto: %1 is not a valid Win32 application`

This error, while distinct from the null byte issue, is a common `protoc-gen-ts_proto` problem on Windows. It typically occurs when the `protoc --plugin` argument does not correctly specify the path to the `.cmd` file for the plugin [6].

**Solution:**

*   **Explicitly Specify `.cmd` Path:** Ensure the `protoc --plugin` argument points directly to `protoc-gen-ts_proto.cmd` with its full or correct relative path [6].

By systematically applying these solutions and workarounds, the persistent `protoc-gen-ts_proto.bat` null byte corruption and related build failures in Chromium on Windows can be addressed.

## References

[1] Chen, R. (2021, July 26). *Diagnosing why your batch file prints a garbage character, one character, and nothing more*. The Old New Thing. Available from https://devblogs.microsoft.com/oldnewthing/20210726-00/?p=105483

[2] Chromium Issue Tracker. *ACTION //third_party/perfetto/protos/perfetto/trace/chrome:minimal_complete_lite_gen(//build/toolchain/linux:clang_x64) print to stdout [406928883]*. Available from https://issues.chromium.org/issues/406928883

[3] Chromium. *Ninja Build*. Available from https://chromium.googlesource.com/chromium/src/+/0e94f26e8/docs/ninja_build.md

[4] Chromium Issue Tracker. *Can't build on Windows: Protoc has returned non-zero status [40513764]*. Available from https://issues.chromium.org/issues/40513764

[5] Stack Overflow. (2020, December 21). *how can we set DEPOT_TOOLS_WIN_TOOLCHAIN=0?* [Online forum post]. Available from https://stackoverflow.com/questions/65390174/how-can-we-set-depot-tools-win-toolchain-0

[6] Stack Overflow. (2023, November 27). *when I use protoc gave me the following error "--ts_proto_out: protoc-gen-ts_proto: %1 is not a valid Win32 application". Anyone knows why?* [Online forum post]. Available from https://stackoverflow.com/questions/7759843/when-i-use-protoc-gave-me-the-following-error-ts-proto-out-protoc-gen-ts-pro
