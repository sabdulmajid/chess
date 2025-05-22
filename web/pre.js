// Pre-load JavaScript for Emscripten module
window.Module = {
    onRuntimeInitialized: function() {
        console.log('Chess Engine WebAssembly module loaded');
        if (window.chessGame && window.chessGame.init) {
            window.chessGame.init();
        }
    },
    print: function(text) {
        console.log('Chess Engine:', text);
    },
    printErr: function(text) {
        console.error('Chess Engine Error:', text);
    }
};
