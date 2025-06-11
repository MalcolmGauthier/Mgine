#include "MG_main.h"

int MG_sdl_init(MG_Instance* window, SDL_GLContext gl_context);
int MG_gl_init(SDL_GLContext gl_context);
void MG_instance_init(MG_Instance* instance);

int main(int argc, char* argv[])
{
    MG_Instance inst = { 0 };
	SDL_Thread* event_thread = NULL;
	SDL_Thread* logic_thread = NULL;
	SDL_Thread* render_thread = NULL;

    if (MG_sdl_init(&inst, inst.gl_context))
    {
		printf("Failed to initialize SDL\n");
		return -1;
    }

    if (MG_gl_init(inst.gl_context))
    {
        printf("Failed to initialize OpenGL\n");
        SDL_DestroyWindow(inst.window);
        SDL_Quit();
        return -2;
    }

	MG_instance_init(&inst);

	event_thread = SDL_CreateThread(MG_window_loop, "MGine WindowEvents", &inst);
	logic_thread = SDL_CreateThread(MG_logic_loop, "MGine Logic", &inst);
	render_thread = SDL_CreateThread(MG_render_loop, "MGine Render", &inst);

    while (inst.active)
    {
		if (inst.gl_error_code = glGetError())
		{
            switch (inst.gl_error_code)
            {
            case GL_INVALID_ENUM:
                printf("OpenGL Error: Invalid enum — An unacceptable value was specified for an enumerated argument.");
                break;
            case GL_INVALID_VALUE:
                printf("OpenGL Error: Invalid value — A numeric argument is out of range.");
                break;
            case GL_INVALID_OPERATION:
                printf("OpenGL Error: Invalid operation — The specified operation is not allowed in the current state.");
                break;
            case GL_STACK_OVERFLOW:
                printf("OpenGL Error: Stack overflow — This command would cause a stack overflow.");
                break;
            case GL_STACK_UNDERFLOW:
                printf("OpenGL Error: Stack underflow — This command would cause a stack underflow.");
                break;
            case GL_OUT_OF_MEMORY:
                printf("OpenGL Error: Out of memory.");
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                printf("OpenGL Error: Invalid framebuffer operation — The framebuffer object is not complete.");
                break;
            default:
                printf("OpenGL Error: Unknown error code: 0x%X\n", inst.gl_error_code);
                break;
            }
			inst.active = 0;
		}

		SDL_PumpEvents();
        SDL_Delay(1);
    }

	SDL_WaitThread(event_thread, NULL);
	SDL_WaitThread(logic_thread, NULL);
	SDL_WaitThread(render_thread, NULL);
    SDL_GL_DeleteContext(inst.gl_context);
    SDL_DestroyWindow(inst.window);
    SDL_Quit();
    return 0;
}

int MG_sdl_init(MG_Instance* instance, SDL_GLContext gl_context)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    instance->window = SDL_CreateWindow(MG_W_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        MG_W_WIDTH, MG_W_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | MG_W_EXTRA_FLAGS
    );

    if (!instance->window)
    {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -2;
    }

    gl_context = SDL_GL_CreateContext(instance->window);
    if (!gl_context)
    {
        printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(instance->window);
        SDL_Quit();
        return -3;
    }

    return 0;
}

int MG_gl_init(SDL_GLContext gl_context)
{
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        SDL_GL_DeleteContext(gl_context);
        return -1;
    }

    const GLubyte* ver = glGetString(GL_VERSION);
    printf("OpenGL version: %s\n", ver);

    glViewport(0, 0, MG_W_WIDTH, MG_W_HEIGHT);

    return 0;
}

void MG_instance_init(MG_Instance* instance)
{
	if (!instance)
		return;

    instance->active = true;

	instance->window_data.instance = instance;
    instance->window_data.width = MG_W_WIDTH;
	instance->window_data.height = MG_W_HEIGHT;
	instance->window_data.windowed_mode = true;
	instance->window_data.focused = true;

	instance->game_data.tickrate = MG_L_TRICKRATE;
	instance->game_data.global_timer = 0;
	instance->game_data.next_object_id = 1;
	instance->game_data.object_count = 0;
	instance->game_data.object_list = NULL;

	instance->render_data.instance = instance;
	instance->render_data.transparency_list = NULL;
}