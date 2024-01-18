import CV8

public class JSContext {
    let isolate: UnsafeMutableRawPointer
    let context: UnsafeMutableRawPointer
    var template: UnsafeMutableRawPointer

    let unowned: Bool

    public init(
        isolate: UnsafeMutableRawPointer,
        context: UnsafeMutableRawPointer,
        template: UnsafeMutableRawPointer)
    {
        self.unowned = true
        self.isolate = isolate
        self.context = context
        self.template = template
    }

    public init(isolate: UnsafeMutableRawPointer) {
        self.unowned = false
        self.isolate = isolate
        self.template = CV8.createTemplate(isolate)
        self.context = CV8.createContext(isolate, template)
    }

    deinit {
        if !unowned {
            CV8.disposeTemplate(template)
            CV8.disposeContext(context)
        }
    }
}

struct JSError: Error, Equatable, CustomStringConvertible {
    let description: String

    init(_ description: String) {
        self.description = description
    }
}

extension JSContext: JavaScript.JSContext {
    @discardableResult
    public func evaluate(_ script: String) throws -> JSValue {
        var exception: UnsafeMutableRawPointer?
        var count: Int = 0
        guard 
            let pointer =
                CV8.evaluate(isolate, context, script, &exception, &count)
        else {
            guard let exception else {
                fatalError("exception pointer is nil")
            }
            let buffer = UnsafeRawBufferPointer(start: exception, count: count)
            throw JSError(String(decoding: buffer, as: UTF8.self))
        }
        return JSValue(isolate: isolate, pointer: pointer)
    }
}
