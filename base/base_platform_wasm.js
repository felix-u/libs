export async function start(container) {
    const canvas = document.createElement("canvas");
    canvas.style.display = "block";
    canvas.style.width = "100%";
    canvas.style.height = "100%";
    canvas.style.outline = "none";
    canvas.tabIndex = 0;
    container.append(canvas);

    const gl = canvas.getContext("webgl2", { alpha: false, antialias: false, depth: true, stencil: true });
    if (gl === null) {
        container.textContent = "This program needs WebGL2, which is not available in this browser.";
        return;
    }
    gl.getExtension("EXT_color_buffer_float");

    let wasm = null;
    const memory_u8 = () => new Uint8Array(wasm.instance.exports.memory.buffer);
    const memory_i32 = () => new Int32Array(wasm.instance.exports.memory.buffer);
    const memory_u32 = () => new Uint32Array(wasm.instance.exports.memory.buffer);
    const read_string = (pointer, count) => new TextDecoder().decode(memory_u8().subarray(pointer, pointer + count));
    const read_cstring = pointer => {
        const bytes = memory_u8();
        let end = pointer;
        while (bytes[end] != 0) end += 1;
        return read_string(pointer, end - pointer);
    };

    const gl_objects = [null];
    const gl_uniform_locations = [null];
    const gl_object_add = object => {
        gl_objects.push(object);
        return gl_objects.length - 1;
    };
    const gl_generate = (count, pointer, create) => {
        const ids = memory_i32();
        for (let i = 0; i < count; i += 1) ids[(pointer >> 2) + i] = gl_object_add(create());
    };
    const gl_delete = (count, pointer, destroy) => {
        const ids = memory_i32();
        for (let i = 0; i < count; i += 1) {
            const id = ids[(pointer >> 2) + i];
            destroy(gl_objects[id]);
            gl_objects[id] = null;
        }
    };
    const gl_delete_one = (id, destroy) => {
        destroy(gl_objects[id]);
        gl_objects[id] = null;
    };
    const GL_FLOAT = 0x1406;
    const GL_HALF_FLOAT = 0x140B;
    const GL_INFO_LOG_LENGTH = 0x8B84;
    const gl_pixel_view = type => {
        const buffer = wasm.instance.exports.memory.buffer;
        let view = new Uint8Array(buffer);
        if (type == GL_HALF_FLOAT) view = new Uint16Array(buffer);
        if (type == GL_FLOAT) view = new Float32Array(buffer);
        return view;
    };
    const gl_pixel_offset = (type, pointer) => pointer / gl_pixel_view(type).BYTES_PER_ELEMENT;
    const gl_integer_from_parameter = value => {
        let integer = value;
        if (value === true) integer = 1;
        if (value === false) integer = 0;
        return integer;
    };
    const gl_write_info_log = (log, capacity, length_pointer, log_pointer) => {
        const bytes = new TextEncoder().encode(log ?? "");
        const count = Math.max(0, Math.min(bytes.length, capacity - 1));
        const memory = memory_u8();
        memory.set(bytes.subarray(0, count), log_pointer);
        memory[log_pointer + count] = 0;
        if (length_pointer != 0) memory_i32()[length_pointer >> 2] = count;
    };

    const gl_imports = {
        glActiveTexture: texture => gl.activeTexture(texture),
        glAttachShader: (program, shader) => gl.attachShader(gl_objects[program], gl_objects[shader]),
        glBindBuffer: (target, buffer) => gl.bindBuffer(target, gl_objects[buffer]),
        glBindBufferRange: (target, index, buffer, offset, size) => gl.bindBufferRange(target, index, gl_objects[buffer], offset, size),
        glBindFramebuffer: (target, framebuffer) => gl.bindFramebuffer(target, gl_objects[framebuffer]),
        glBindRenderbuffer: (target, renderbuffer) => gl.bindRenderbuffer(target, gl_objects[renderbuffer]),
        glBindTexture: (target, texture) => gl.bindTexture(target, gl_objects[texture]),
        glBindVertexArray: array => gl.bindVertexArray(gl_objects[array]),
        glBlendFunc: (source, destination) => gl.blendFunc(source, destination),
        glBufferData: (target, size, data, usage) => {
            if (data == 0) gl.bufferData(target, size, usage);
            else gl.bufferData(target, memory_u8(), usage, data, size);
        },
        glCheckFramebufferStatus: target => gl.checkFramebufferStatus(target),
        glClear: mask => gl.clear(mask),
        glClearColor: (red, green, blue, alpha) => gl.clearColor(red, green, blue, alpha),
        glCompileShader: shader => gl.compileShader(gl_objects[shader]),
        glCreateProgram: () => gl_object_add(gl.createProgram()),
        glCreateShader: type => gl_object_add(gl.createShader(type)),
        glCullFace: mode => gl.cullFace(mode),
        glDebugMessageCallbackKHR: () => {},
        glDeleteBuffers: (count, pointer) => gl_delete(count, pointer, object => gl.deleteBuffer(object)),
        glDeleteFramebuffers: (count, pointer) => gl_delete(count, pointer, object => gl.deleteFramebuffer(object)),
        glDeleteProgram: program => gl_delete_one(program, object => gl.deleteProgram(object)),
        glDeleteRenderbuffers: (count, pointer) => gl_delete(count, pointer, object => gl.deleteRenderbuffer(object)),
        glDeleteShader: shader => gl_delete_one(shader, object => gl.deleteShader(object)),
        glDeleteTextures: (count, pointer) => gl_delete(count, pointer, object => gl.deleteTexture(object)),
        glDepthFunc: function_ => gl.depthFunc(function_),
        glDepthMask: flag => gl.depthMask(flag != 0),
        glDisable: capability => gl.disable(capability),
        glDisableVertexAttribArray: index => gl.disableVertexAttribArray(index),
        glDrawArraysInstanced: (mode, first, count, instance_count) => gl.drawArraysInstanced(mode, first, count, instance_count),
        glDrawElementsInstanced: (mode, count, type, offset, instance_count) => gl.drawElementsInstanced(mode, count, type, offset, instance_count),
        glEnable: capability => gl.enable(capability),
        glEnableVertexAttribArray: index => gl.enableVertexAttribArray(index),
        glFramebufferRenderbuffer: (target, attachment, renderbuffer_target, renderbuffer) => gl.framebufferRenderbuffer(target, attachment, renderbuffer_target, gl_objects[renderbuffer]),
        glFramebufferTexture2D: (target, attachment, texture_target, texture, level) => gl.framebufferTexture2D(target, attachment, texture_target, gl_objects[texture], level),
        glGenBuffers: (count, pointer) => gl_generate(count, pointer, () => gl.createBuffer()),
        glGenFramebuffers: (count, pointer) => gl_generate(count, pointer, () => gl.createFramebuffer()),
        glGenRenderbuffers: (count, pointer) => gl_generate(count, pointer, () => gl.createRenderbuffer()),
        glGenTextures: (count, pointer) => gl_generate(count, pointer, () => gl.createTexture()),
        glGenVertexArrays: (count, pointer) => gl_generate(count, pointer, () => gl.createVertexArray()),
        glGenerateMipmap: target => gl.generateMipmap(target),
        glGetActiveUniformBlockiv: (program, index, name, pointer) => {
            memory_i32()[pointer >> 2] = gl_integer_from_parameter(gl.getActiveUniformBlockParameter(gl_objects[program], index, name));
        },
        glGetIntegerv: (name, pointer) => {
            memory_i32()[pointer >> 2] = gl_integer_from_parameter(gl.getParameter(name));
        },
        glGetProgramInfoLog: (program, capacity, length_pointer, log_pointer) => gl_write_info_log(gl.getProgramInfoLog(gl_objects[program]), capacity, length_pointer, log_pointer),
        glGetProgramiv: (program, name, pointer) => {
            let value = 0;
            if (name == GL_INFO_LOG_LENGTH) value = (gl.getProgramInfoLog(gl_objects[program]) ?? "").length + 1;
            else value = gl.getProgramParameter(gl_objects[program], name);
            memory_i32()[pointer >> 2] = gl_integer_from_parameter(value);
        },
        glGetShaderInfoLog: (shader, capacity, length_pointer, log_pointer) => gl_write_info_log(gl.getShaderInfoLog(gl_objects[shader]), capacity, length_pointer, log_pointer),
        glGetShaderiv: (shader, name, pointer) => {
            let value = 0;
            if (name == GL_INFO_LOG_LENGTH) value = (gl.getShaderInfoLog(gl_objects[shader]) ?? "").length + 1;
            else value = gl.getShaderParameter(gl_objects[shader], name);
            memory_i32()[pointer >> 2] = gl_integer_from_parameter(value);
        },
        glGetUniformBlockIndex: (program, name) => gl.getUniformBlockIndex(gl_objects[program], read_cstring(name)),
        glGetUniformLocation: (program, name) => {
            const location = gl.getUniformLocation(gl_objects[program], read_cstring(name));
            let id = -1;
            if (location !== null) {
                gl_uniform_locations.push(location);
                id = gl_uniform_locations.length - 1;
            }
            return id;
        },
        glLinkProgram: program => gl.linkProgram(gl_objects[program]),
        glPixelStorei: (name, value) => gl.pixelStorei(name, value),
        glReadPixels: (x, y, width, height, format, type, pointer) => gl.readPixels(x, y, width, height, format, type, gl_pixel_view(type), gl_pixel_offset(type, pointer)),
        glRenderbufferStorage: (target, format, width, height) => gl.renderbufferStorage(target, format, width, height),
        glScissor: (x, y, width, height) => gl.scissor(x, y, width, height),
        glShaderSource: (shader, count, strings_pointer, lengths_pointer) => {
            const pointers = memory_u32();
            const lengths = memory_i32();
            let source = "";
            for (let i = 0; i < count; i += 1) {
                const pointer = pointers[(strings_pointer >> 2) + i];
                const length = lengths_pointer == 0 ? -1 : lengths[(lengths_pointer >> 2) + i];
                source += length < 0 ? read_cstring(pointer) : read_string(pointer, length);
            }
            gl.shaderSource(gl_objects[shader], source);
        },
        glTexImage2D: (target, level, internal_format, width, height, border, format, type, pixels) => {
            if (pixels == 0) gl.texImage2D(target, level, internal_format, width, height, border, format, type, null);
            else gl.texImage2D(target, level, internal_format, width, height, border, format, type, gl_pixel_view(type), gl_pixel_offset(type, pixels));
        },
        glTexParameteri: (target, name, value) => gl.texParameteri(target, name, value),
        glTexSubImage2D: (target, level, x, y, width, height, format, type, pixels) => gl.texSubImage2D(target, level, x, y, width, height, format, type, gl_pixel_view(type), gl_pixel_offset(type, pixels)),
        glUniform1i: (location, value) => gl.uniform1i(gl_uniform_locations[location] ?? null, value),
        glUniform2f: (location, x, y) => gl.uniform2f(gl_uniform_locations[location] ?? null, x, y),
        glUniformBlockBinding: (program, index, binding) => gl.uniformBlockBinding(gl_objects[program], index, binding),
        glUseProgram: program => gl.useProgram(gl_objects[program]),
        glVertexAttribDivisor: (index, divisor) => gl.vertexAttribDivisor(index, divisor),
        glVertexAttribIPointer: (index, size, type, stride, offset) => gl.vertexAttribIPointer(index, size, type, stride, offset),
        glVertexAttribPointer: (index, size, type, normalised, stride, offset) => gl.vertexAttribPointer(index, size, type, normalised != 0, stride, offset),
        glViewport: (x, y, width, height) => gl.viewport(x, y, width, height),
    };

    const imports = { env: {
        ...gl_imports,
        js_file_write: (name_cstring, pointer, count) => {
            const bytes = memory_u8().slice(pointer, pointer + count);
            const name = read_cstring(name_cstring);

            console.assert(!name.includes("/") && !name.includes("\\"), "js_file_write: path separators forbidden in name", name);

            const blob = new Blob([bytes]);
            const a = document.createElement("a");
            a.href = URL.createObjectURL(blob);
            a.download = name;
            a.click();
            URL.revokeObjectURL(a.href);
        },
        js_performance_now: () => {
            return performance.now();
        },
        js_set_cursor_hidden: hidden => {
            canvas.style.cursor = hidden ? "none" : "";
        },
        os_write_console_js: (pointer, count) => {
            console.log(read_string(pointer, count));
        },
    } };

    // TODO(felix): make .wasm file name changeable in build
    wasm = await WebAssembly.instantiateStreaming(fetch(new URL("module.wasm", import.meta.url)), imports);
    const exports = wasm.instance.exports;

    exports.platform__start();

    let mouse_x = 0, mouse_y = 0;
    canvas.addEventListener("mousemove", e => {
        const rect = canvas.getBoundingClientRect();
        mouse_x = (e.clientX - rect.left) * devicePixelRatio;
        mouse_y = (e.clientY - rect.top) * devicePixelRatio;
    });

    function write_js_string(string) {
        const bytes = new TextEncoder().encode(string);
        const buffer = exports.platform__get_temp_js_string_buffer();
        memory_u8().set(bytes, buffer);
        return [buffer, bytes.length];
    }
    canvas.addEventListener("keydown", e => {
        const [buffer, length] = write_js_string(e.code);
        exports.platform__on_key_down(buffer, length);
    });
    canvas.addEventListener("keyup", e => {
        const [buffer, length] = write_js_string(e.code);
        exports.platform__on_key_up(buffer, length);
    });

    canvas.addEventListener("mousedown", e => exports.platform__on_mouse_down(e.button));
    canvas.addEventListener("mouseup", e => exports.platform__on_mouse_up(e.button));
    canvas.addEventListener("wheel", e => {
        const has_focus = document.activeElement === canvas;
        if (has_focus) {
            e.preventDefault();
            exports.platform__on_scroll(e.deltaX, e.deltaY);
        }
    }, { passive: false });

    function frame() {
        const real_width = Math.round(canvas.clientWidth * devicePixelRatio);
        const real_height = Math.round(canvas.clientHeight * devicePixelRatio);
        if (canvas.width != real_width || canvas.height != real_height) {
            canvas.width = real_width;
            canvas.height = real_height;
        }

        exports.platform__update_and_render(real_width, real_height, mouse_x, mouse_y);
        requestAnimationFrame(frame);
    }
    requestAnimationFrame(frame);
}
