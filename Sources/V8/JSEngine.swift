@_exported import JavaScript

public class JSEngine: JavaScript.JSEngine {
    public typealias JSRuntime = V8.JSRuntime
    public static func createRuntime() -> JSRuntime {
        return JSRuntime()
    }
}
