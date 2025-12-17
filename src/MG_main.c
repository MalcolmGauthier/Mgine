/////////////////////////////////
//                             //
//  MGINE BY MALCOLM GAUTHIER  //
//             2025            //
//                             //
/////////////////////////////////

#include "MG_main.h"

static int MG_sdl_init(MG_Instance* window, SDL_GLContext gl_context, bool no_window, bool sub_instance);
static int MG_gl_init(SDL_GLContext gl_context);
static void MG_instance_init(MG_Instance* instance);
static void MG_instance_free(MG_Instance* instance);

int main(int argc, char** argv)
{
    // There are no global variables in Mgine to leave the door open for the engine 
    // being able to create new instances of itself in the future.
    MG_Instance inst = { 0 };

    // Mgine operates on 3 threads (4 if you include this one, which is the debug/root thread). 
    // The event thread handles I/O. It responds to window events and any keyboard/mouse/controller inputs.
    // The logic thread runs the game's logic. By default it runs at 60 ticks per second.
    // The render thread renders the game to the screen at a rate hopefully higher than that of the logic thread, using interpolation.
	SDL_Thread* event_thread = NULL;
	SDL_Thread* logic_thread = NULL;
	SDL_Thread* render_thread = NULL;

    if (MG_sdl_init(&inst, inst.gl_context, false, false))
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
    MG_initialize_components(&inst);
    MG_load_game(&inst);

    // windows is stupid, and without this any sleep calls cannot be guarenteed to last less than ~16ms
    // a single call to this is enough to set the option for all threads, but i couldn't cleanly fit this into the main thread
    timeBeginPeriod(1);

    // here the threads are created. This main thread here counts as a 4th thread, but it's only used for
	// debugging and error monitoring.
	event_thread = SDL_CreateThread(MG_window_loop, "MGine WindowEvents", &inst);
	logic_thread = SDL_CreateThread(MG_logic_loop, "MGine Logic", &inst);
	render_thread = SDL_CreateThread(MG_render_loop, "MGine Render", &inst);

    inst.rendering_enabled = false;

    // This loop here checks for GL errors at the moment. Debug tools could be added in the future.
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

		// Without this, the event loop will not be able to process events properly.
		SDL_PumpEvents();
        SDL_Delay(1);
    }

	SDL_WaitThread(event_thread, NULL);
	SDL_WaitThread(logic_thread, NULL);
	SDL_WaitThread(render_thread, NULL);
    SDL_GL_DeleteContext(inst.gl_context);
    SDL_DestroyWindow(inst.window);
    MG_instance_free(&inst);
    SDL_Quit();
    timeEndPeriod(1);
    return 0;
}

// creates a new instance of the engine, using the memory provided in out_instance.
// this uses alot of memory and processing power, whatever you're doing better be worth it.
// turning on no_window will make this instance invisible and in the background.
// it is highly advised to not lose track of the pointer to this, and to make sure to free it by changing the active boolean.
// code-wise, it's nearly indentical to main. I would've had main just call this function then do nothing, but that would be a waste of a root thread.
// AUDIO IS NOT AVAILABLE IN SUB-INSTANCES.
void MG_create_instance(MG_Instance* out_instance, bool no_window)
{
    MG_Instance* inst;
    // if out_instance is null, the creator cannot keep track of it. very unlikely to be useful, so we don't risk a memory leak.
    if (!out_instance)
        return;

    // zero out to prevent initialization errors. it would be cool to provide your own starting point, but then
    // dumb people could mess with the gl_context pointers or something. plus, i don't want to take the time to verify that
    // doing such a stupid move would always work.
    *out_instance = (MG_Instance){ 0 };
    inst = out_instance;

    SDL_Thread* event_thread = NULL;
    SDL_Thread* logic_thread = NULL;
    SDL_Thread* render_thread = NULL;

    if (MG_sdl_init(inst, inst->gl_context, no_window, true))
    {
        printf("Failed to initialize SDL\n");
        inst->instance_exit_code = -1;
        return;
    }

    if (!no_window && MG_gl_init(inst->gl_context))
    {
        printf("Failed to initialize OpenGL\n");
        SDL_DestroyWindow(inst->window);
        SDL_Quit();
        inst->instance_exit_code = -2;
        return;
    }

    MG_instance_init(inst);

    event_thread = SDL_CreateThread(MG_window_loop, "MGine WindowEvents", inst);
    logic_thread = SDL_CreateThread(MG_logic_loop, "MGine Logic", inst);
    if (!no_window)
        render_thread = SDL_CreateThread(MG_render_loop, "MGine Render", inst);

    // no debugging in child instances
    while (inst->active)
    {
        SDL_PumpEvents();
        SDL_Delay(1);
    }

    SDL_WaitThread(event_thread, NULL);
    SDL_WaitThread(logic_thread, NULL);
    if (!no_window)
    {
        SDL_WaitThread(render_thread, NULL);
        SDL_GL_DeleteContext(inst->gl_context);
        SDL_DestroyWindow(inst->window);
    }
    SDL_Quit();
    free(inst);
    return;
}

// Initializes SDL and creates a window with an OpenGL context.
static int MG_sdl_init(MG_Instance* instance, SDL_GLContext gl_context, bool no_window, bool sub_instance)
{
	int error_code = 0;

    if (!sub_instance)
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
        {
            printf("SDL_Init failed: %s\n", SDL_GetError());
            return -1;
        }
    }

    if (no_window)
        return 0;

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
        error_code = -2;
		goto fail;
    }

    gl_context = SDL_GL_CreateContext(instance->window);
    if (!gl_context)
    {
        printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
		error_code = -3;
		goto fail2;
    }

    if (sub_instance)
		return 0;

    if (!Mix_Init(0xFF))
    {
        printf("SDL Mixer init failed: %s\n", Mix_GetError());
		error_code = -4;
		goto fail3;
    }
    if (!Mix_OpenAudio(MG_A_OUTPUT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048))
    {
        printf("SDL Mixer audio init failed: %s\n", Mix_GetError());
        error_code = -5;
        goto fail4;
    }
	Mix_AllocateChannels(MG_A_CHANNEL_COUNT);

    return 0;

fail4:
	Mix_Quit();
fail3:
    SDL_GL_DeleteContext(gl_context);
fail2:
    SDL_DestroyWindow(instance->window);
fail:
    SDL_Quit();
    return error_code;
}

// Initializes OpenGL using GLAD and sets the viewport.
static int MG_gl_init(SDL_GLContext gl_context)
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

// Initializes the MG_Instance structure with initialization values.
static void MG_instance_init(MG_Instance* instance)
{
	if (!instance)
		return;

	instance->window_data.instance = instance;
    instance->window_data.width = MG_W_WIDTH;
	instance->window_data.height = MG_W_HEIGHT;
	instance->window_data.windowed_mode = true;
	instance->window_data.focused = true;

	instance->game_data.instance = instance;
	instance->game_data.tickrate = MG_L_TRICKRATE;
	instance->game_data.global_timer = 0;
	instance->game_data.next_object_id = 1;
	instance->game_data.object_count = 0;
	instance->game_data.object_list = NULL;

    extern MG_Audio* mg_audio;
	mg_audio = &instance->audio_data;
	instance->audio_data.instance = instance;
    instance->audio_data.ears = &instance->game_data.camera.position;
	instance->audio_data.ears_yaw = &instance->game_data.camera.rotation.y;
	Mix_ChannelFinished(MG_audio_free_channel);

	instance->render_data.instance = instance;
	instance->render_data.transparency_list = NULL;

    instance->active = true;
    QueryPerformanceCounter((LARGE_INTEGER*)&instance->instance_id);
}

// Frees the memory used by the instance (objects, components, data, etc.)
static void MG_instance_free(MG_Instance* instance)
{
    //TODO
}
