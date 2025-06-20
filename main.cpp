#include <emscripten.h>
#include <emscripten/html5.h>
#include <math.h>

int x = 0;

void update() {
    x = (x + 1) % 500;
    EM_ASM({
        const canvas = document.getElementById('canvas');
        const ctx = canvas.getContext('2d');
        ctx.clearRect(0, 0, 500, 500);
        ctx.beginPath();
        ctx.arc($0, 250, 20, 0, 2 * Math.PI);
        ctx.fillStyle = "red";
        ctx.fill();
    }, x);
}

int main() {
    emscripten_set_main_loop(update, 60, 1);
    return 0;
}
