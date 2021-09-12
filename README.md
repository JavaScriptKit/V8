# V8

## Package.swift

```swift
.package(url: "https://github.com/JavaScriptKit/V8.git", .branch("dev"))
```

## Usage

```swift
let context = JSContext()
try context.evaluate("40 + 2")

try context.createFunction(name: "getResult") {
    return .string("result string")
}
let result = try context.evaluate("getResult()")
expect(result.isString)
expect(try result.toString() == "result string")
expect("\(result)" == "result string")
```

## Requirements

### macOS

```bash
brew install v8
```
#### SwiftPM arguments

```bash
swift build -Xcc -I/usr/local/Cellar/v8/9.3.345.16/libexec/include 

swift test \
 -Xcc -I/usr/local/Cellar/v8/9.3.345.16/libexec/include \
 -Xlinker -L/usr/local/Cellar/v8/9.3.345.16/libexec \
 --generate-linuxmain

swift package \
 -Xcc -I/usr/local/Cellar/v8/9.3.345.16/libexec/include \
 -Xlinker -L/usr/local/Cellar/v8/9.3.345.16/libexec \
 generate-xcodeproj
```

### Linux

For full instructions follow https://v8.dev/docs/build
```bash
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
export PATH=$PATH:$(pwd)/depot_tools
fetch v8
cd v8
gclient sync -r 9.3.345.16
./build/install-build-deps.sh #OMG
gn gen --args="is_debug=false is_component_build=true v8_use_external_startup_data=false v8_enable_i18n_support=false" out.gn/x64.release
ninja -j8 -C out.gn/x64.release -v d8
```

#### SwiftPM arguments

```bash
export LD_LIBRARY_PATH=/opt/libv8-9.3/lib
swift build -Xcc -I/opt/libv8-9.3/include -Xlinker -L/opt/libv8-9.3/lib
swift test -Xcc -I/opt/libv8-9.3/include -Xlinker -L/opt/libv8-9.3/lib
```
