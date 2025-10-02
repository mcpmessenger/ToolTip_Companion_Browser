## Building Individual Chromium Components with Ninja

Chromium's build system, Ninja, allows for building specific components or targets rather than the entire browser. This approach can significantly reduce build times and help isolate issues related to specific parts of the codebase, such as the `ToolTip_Companion_Browser`.

**Key Principles:**

*   **Targeted Builds:** Instead of building all targets (`ninja -C out/Debug all`), developers can specify individual targets to build. For example, to build the `chrome` executable, the command would be `ninja -C out/Debug chrome`.
*   **Identifying Targets:** The challenge lies in identifying the correct Ninja target name for the desired component. This often corresponds to the directory structure or the `gn` target definitions within the Chromium source tree. For a component like `ToolTip_Companion_Browser`, the target name might be `tooltip_components` or a more specific target within that directory, such as `chrome_browser_tooltip_components`.
*   **Component Builds (`is_component_build`):** The `is_component_build = true` GN argument (often the default for debug builds) links many parts of Chrome into separate shared libraries. While this speeds up linking, it's distinct from building only a *subset* of targets. Building a subset still uses the component build structure if `is_component_build` is true.

**Steps for Building Individual Components:**

1.  **Configure `gn args`:** Set up your `args.gn` file in your build directory (e.g., `out/Default`) with the desired build flags (e.g., `is_debug = true`, `is_component_build = true`).
2.  **Generate Ninja Files:** Run `gn gen out/Default` to generate the Ninja build files based on your `gn args`.
3.  **Identify the Target:** This is the most crucial step. You would need to explore the `BUILD.gn` files within the `tooltip_components/chrome/browser` directory (or similar relevant paths) to find the defined `executable`, `static_library`, or `shared_library` targets for the `ToolTip_Companion_Browser`.
    *   A common way to find targets is to use `gn refs <out_dir> <target_pattern>` or examine the `build.ninja` files directly after generation.
4.  **Execute Ninja for the Specific Target:** Once the target name is identified (e.g., `tooltip_browser_component`), execute Ninja:
    ```bash
    ninja -C out/Default tooltip_browser_component
    ```

This approach allows for a more granular build process, potentially avoiding problematic `protoc` generations that are not directly relevant to the `ToolTip_Companion_Browser` component.
