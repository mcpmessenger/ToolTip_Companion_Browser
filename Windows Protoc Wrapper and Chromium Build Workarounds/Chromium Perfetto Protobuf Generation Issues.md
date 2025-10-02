## Chromium Perfetto Protobuf Generation Issues

Chromium issue 406928883 highlights problems related to Perfetto protobuf generation, specifically concerning `protoc` invocations and protobuf rules within the Chromium build system. While this issue was marked as fixed, it provides valuable insights into potential areas where `protoc` can cause build failures.

**Key Observations:**

*   **`protoc` Invocations and Build Errors:** The issue describes instances where `protoc` invocations for Perfetto protobufs were printing to `stdout` but not failing the build, which was considered a bug. The goal was to make these print statements trigger build errors, indicating a desire for stricter enforcement of `protoc` output [1].
*   **`--fatal-warnings` Flag:** It was noted that `protoc` now supports a `--fatal-warnings` flag, which could be injected into `protoc` arguments to treat warnings as errors. This suggests a mechanism to make `protoc` failures more explicit [1].
*   **Perfetto Protobuf Rules:** The discussion points to `third_party/protobuf/proto_library.gni` as the place where `protoc` arguments might need to be modified for Perfetto protobuf rules, indicating that Perfetto's protobuf generation often defers to Chromium's general protobuf rules [1].
*   **Complexity of Protobufs in Chromium:** The issue acknowledges the vast number of protobufs within Chromium, making it challenging to track all `protoc` invocations and potential failure points [1].

**Potential Workarounds/Solutions (based on the issue context):**

1.  **Examine `third_party/protobuf/proto_library.gni`:** While direct modification of core Chromium build files is generally discouraged, understanding how `protoc` arguments are passed for protobuf rules in this file could be crucial. It might reveal opportunities to inject custom flags or modify behavior if a local patch is necessary.
2.  **Identify and Disable Perfetto-related Targets:** If the `protoc-gen-ts_proto.bat` issue is specifically tied to Perfetto's TypeScript generation, identifying the GN targets responsible for this generation and attempting to disable them might be a workaround. This would involve searching `BUILD.gn` files within `third_party/perfetto` or related directories for targets that generate TypeScript from `.proto` files.
3.  **Look for `enable_perfetto_tracing` or similar GN args:** Although no direct `gn arg` like `disable_perfetto_typescript_generation` was found in the general search, specific flags might exist within the Perfetto or Chromium source code to control its generation. Searching the Chromium source for `enable_perfetto_tracing` or similar flags could reveal relevant configuration options.

This issue suggests that `protoc` issues related to Perfetto are a known concern within Chromium development, and while a specific fix was implemented, the underlying mechanisms for protobuf generation remain complex. Troubleshooting should involve a deep dive into the relevant `BUILD.gn` files and potential GN arguments to control Perfetto's protobuf output.

## References

[1] Chromium Issue Tracker. *ACTION //third_party/perfetto/protos/perfetto/trace/chrome:minimal_complete_lite_gen(//build/toolchain/linux:clang_x64) print to stdout [406928883]*. Available from https://issues.chromium.org/issues/406928883
