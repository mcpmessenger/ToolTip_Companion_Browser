## Solution for "protoc-gen-ts_proto: %1 is not a valid Win32 application" error

When encountering the error `--ts_proto_out: protoc-gen-ts_proto: %1 is not a valid Win32 application` on Windows, the solution is to explicitly specify the path to the `.cmd` file for `protoc-gen-ts_proto` in the `protoc --plugin` argument.

**Original command causing the error:**
```bash
protoc --plugin=./node_modules/.bin/protoc-gen-ts_proto --ts_proto_out=./ --ts_proto_opt=nestJs=true ./src/proto/com.proto
```

**Corrected command:**
```bash
protoc --plugin=protoc-gen-ts_proto="C:\path\to\your\project\folder\node_modules\.bin\protoc-gen-ts_proto.cmd" --ts_proto_out=./ --ts_proto_opt=nestJs=true ./proto/yourfile.proto
```

Alternatively, a relative path to the `.cmd` file might also work:
```bash
protoc --plugin=./node_modules/.bin/protoc-gen-ts_proto.cmd --ts_proto_out=./ --ts_proto_opt=nestJs=true ./proto/yourfile.proto
```

This ensures that the `protoc` command correctly invokes the batch file on Windows systems.
