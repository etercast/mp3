all: mp3.wasm

#
# What are these options for?
#
# -DNDEBUG: Defines NDEBUG to remove asserts.
# -Oz: Optimize for size.
# --prefix: Where are we going to put generated files.
# --host: Indicates what kind of machine (in this wasm) we are using.
# --disable-static: Disables building the static version of libmp3lame.
# --disable-shared: Disables building the shared version of libmp3lame.
# --disable-largefile: Disables handling large files.
# --disable-gtktest: Disables gtktests.
# --disable-analyzer-hooks: Disables hooks.
# --disable-decoder: Disables decoder.
# --disable-frontend: Disables LAME frontend (the binary lame).
#
lame/dist/lib/libmp3lame.a:
	cd lame && \
	emconfigure ./configure \
		CFLAGS="-DNDEBUG -Oz" \
		--prefix="$$(pwd)/dist" \
		--host=x86-none-linux \
		--disable-shared \
		--disable-largefile \
		--disable-gtktest \
		--disable-analyzer-hooks \
		--disable-decoder \
		--disable-frontend && \
  emmake make -j12 && \
	emmake make install

mp3.wasm: lame/dist/lib/libmp3lame.a src/mp3.c
	emcc $^ \
		-Ilame/dist/include \
		-DNDEBUG \
    --llvm-lto 3 \
		-Oz \
		-s STRICT=1 \
    -s STANDALONE_WASM=1 \
		-s MALLOC=emmalloc \
		-s ASSERTIONS=0 \
		-s FILESYSTEM=0 \
		-s INVOKE_RUN=0 \
		-s EXPORT_ES6=1 \
		-s MODULARIZE_INSTANCE=1 \
		-o dist/mp3.wasm

clean: clean-lame clean-wasm

clean-lame:
	cd lame && git clean -dfx

clean-wasm:
	rm -f dist/mp3.* && \
	rm -f src/mp3.js src/mp3.wasm
