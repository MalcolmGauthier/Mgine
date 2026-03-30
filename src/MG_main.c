/////////////////////////////////
//                             //
//  MGINE BY MALCOLM GAUTHIER  //
//        (c) 2025-2026        //
//                             //
/////////////////////////////////

#include "MG_main.h"

static int MG_sdl_init();
static int MG_gl_init();
static void MG_instance_init();
static void MG_instance_free();

extern void MG_object_free_prefab(MG_Object* prefab);

struct MG_Instance* MG_INSTANCE = NULL;
static MG_Instance instance_data = { 0 };

int MG_init()
{
    if (instance_data.active)
    {
		printf("Failed to create instance: instance already active. Exit instance before creating new one.\n");
        MG_INSTANCE->instance_exit_code = 1;
        return 1;
    }
	MG_INSTANCE = &instance_data;
	instance_data = (MG_Instance){ 0 };

    if (MG_sdl_init())
    {
        printf("Failed to initialize SDL\n");
        MG_INSTANCE->instance_exit_code = -1;
        return -1;
    }

    if (!MG_W_NO_WINDOW && MG_gl_init())
    {
        printf("Failed to initialize OpenGL\n");
        SDL_DestroyWindow(MG_INSTANCE->window);
        SDL_Quit();
        MG_INSTANCE->instance_exit_code = -2;
        return -2;
    }
    
    MG_instance_init();
    MG_initialize_components();
	if (MG_USE_MG_FILE)
        MG_load_game();

    // to pass opengl context ownership, this function has to be called with null before another thread calls dibs.
    if (SDL_GL_MakeCurrent(MG_INSTANCE->window, NULL))
		printf("Error: Failed to release OpenGL ownership\n");
    // windows is stupid, and without this any sleep calls cannot be guarenteed to last less than ~16ms
    // a single call to this is enough to set the option for all threads, but i couldn't cleanly fit this into the main thread
    timeBeginPeriod(1);
	atexit(MG_quit);

    // Mgine operates on 3 threads. 
    // The event thread handles I/O. It responds to window events and any keyboard/mouse/controller inputs.
    // The logic thread runs the game's logic. By default it runs at 60 ticks per second.
    // The render thread renders the game to the screen at a rate hopefully higher than that of the logic thread, using interpolation.
    MG_INSTANCE->event_thread = SDL_CreateThread(MG_window_loop, "MGine WindowEvents", MG_INSTANCE);
    MG_INSTANCE->logic_thread = SDL_CreateThread(MG_logic_loop, "MGine Logic", MG_INSTANCE);
    if (!MG_W_NO_WINDOW)
        MG_INSTANCE->render_thread = SDL_CreateThread(MG_render_loop, "MGine Rendering", MG_INSTANCE);

    MG_INSTANCE->rendering_enabled = !MG_W_NO_WINDOW;
    MG_INSTANCE->initialized = true;

    // This loop here checks for GL errors at the moment. Debug tools could be added in the future.
    //while (MG_INSTANCE->active)
    //{
    //    // the call to the gl error check function can't be done here. opengl doesn't support multithreading.
    //    if (MG_INSTANCE->gl_error_code)
    //    {
    //        switch (inst->gl_error_code)
    //        {
    //        case GL_INVALID_ENUM:
    //            printf("OpenGL Error: Invalid enum — An unacceptable value was specified for an enumerated argument.");
    //            break;
    //        case GL_INVALID_VALUE:
    //            printf("OpenGL Error: Invalid value — A numeric argument is out of range.");
    //            break;
    //        case GL_INVALID_OPERATION:
    //            printf("OpenGL Error: Invalid operation — The specified operation is not allowed in the current state.");
    //            break;
    //        case GL_STACK_OVERFLOW:
    //            printf("OpenGL Error: Stack overflow — This command would cause a stack overflow.");
    //            break;
    //        case GL_STACK_UNDERFLOW:
    //            printf("OpenGL Error: Stack underflow — This command would cause a stack underflow.");
    //            break;
    //        case GL_OUT_OF_MEMORY:
    //            printf("OpenGL Error: Out of memory.");
    //            break;
    //        case GL_INVALID_FRAMEBUFFER_OPERATION:
    //            printf("OpenGL Error: Invalid framebuffer operation — The framebuffer object is not complete.");
    //            break;
    //        default:
    //            printf("OpenGL Error: Unknown error code: 0x%X\n", inst->gl_error_code);
    //            break;
    //        }
    // 
    //        inst->gl_error_code = 0;
    //    }
    // 
    //}
}

void MG_ready()
{
    if (!MG_INSTANCE)
        return;

    while (!instance_data.initialized)
		SDL_Delay(1);
}

void MG_quit()
{
    if (!MG_INSTANCE)
        return;

    MG_INSTANCE->active = false;

    SDL_WaitThread(MG_INSTANCE->event_thread, NULL);
    SDL_WaitThread(MG_INSTANCE->logic_thread, NULL);
    if (!MG_W_NO_WINDOW)
        SDL_WaitThread(MG_INSTANCE->render_thread, NULL);

    timeEndPeriod(1);
    MG_instance_free();
    SDL_Quit();
    return 0;
}

// Initializes SDL and creates a window with an OpenGL context.
static int MG_sdl_init()
{
	int error_code = 0;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    if (MG_W_NO_WINDOW)
        return 0;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    MG_INSTANCE->window = SDL_CreateWindow(MG_W_NAME,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        MG_W_WIDTH, MG_W_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | MG_W_EXTRA_FLAGS
    );

    if (!MG_INSTANCE->window)
    {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        error_code = -2;
		goto fail;
    }

    MG_INSTANCE->gl_context = SDL_GL_CreateContext(MG_INSTANCE->window);
    if (!MG_INSTANCE->gl_context)
    {
        printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
		error_code = -3;
		goto fail2;
    }

    //TODO: add more format compatibility
    if (Mix_Init(0xFF))
    {
        printf("SDL Mixer audio type init failed: %s\n", Mix_GetError());
        // doesn't halt sdl
		//error_code = -4;
		//goto fail3;
    }
    if (Mix_OpenAudio(MG_A_OUTPUT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048))
    {
        printf("SDL Mixer audio init failed: %s\n", Mix_GetError());
        error_code = -5;
        goto fail4;
    }
	Mix_AllocateChannels(MG_A_CHANNEL_COUNT);

    return 0;

fail4:
	Mix_Quit();
//fail3:
    SDL_GL_DeleteContext(MG_INSTANCE->gl_context);
fail2:
    SDL_DestroyWindow(MG_INSTANCE->window);
fail:
    SDL_Quit();
    return error_code;
}

// Initializes OpenGL using GLAD and sets the viewport.
static int MG_gl_init()
{
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        SDL_GL_DeleteContext(MG_INSTANCE->gl_context);
        return -1;
    }

    const GLubyte* ver = glGetString(GL_VERSION);
    printf("OpenGL version: %s\n", ver);

    glViewport(0, 0, MG_W_WIDTH, MG_W_HEIGHT);

    return 0;
}

// Initializes the MG_Instance structure with initialization values.
static void MG_instance_init()
{
	if (!MG_INSTANCE)
		return;

    MG_INSTANCE->window_data.width = MG_W_WIDTH;
    MG_INSTANCE->window_data.height = MG_W_HEIGHT;
    MG_INSTANCE->window_data.windowed_mode = true;
    MG_INSTANCE->window_data.focused = true;

    MG_INSTANCE->game_data.tickrate = MG_L_TRICKRATE;
    MG_INSTANCE->game_data.global_timer = 0;
    MG_INSTANCE->game_data.next_object_id = 1;
    MG_INSTANCE->game_data.object_count = 0;
    MG_INSTANCE->game_data.object_list = NULL;

    extern MG_Audio* mg_audio;
	extern void MG_audio_free_channel(int channel);
	mg_audio = &MG_INSTANCE->audio_data;
    MG_INSTANCE->audio_data.ears = &MG_INSTANCE->game_data.camera.position;
    MG_INSTANCE->audio_data.ears_yaw = &MG_INSTANCE->game_data.camera.rotation.y;
	Mix_ChannelFinished(MG_audio_free_channel);

    MG_INSTANCE->render_data.transparency_list = NULL;

    MG_INSTANCE->active = true;
    // instance_id CANNOT be an MG_ID, because this function adds in a 64bit number, which would overflow onto another struct field.
    // learned this the hard way...
    QueryPerformanceCounter((LARGE_INTEGER*)&MG_INSTANCE->instance_id);
}

// Frees the memory used by the instance (objects, components, data, etc.)
static void MG_instance_free()
{
    // use struct instead of pointer to make extra sure memory is freed. there's alot of memory on the table, can't mess this up.
    SDL_GL_DeleteContext(instance_data.gl_context);
    SDL_DestroyWindow(instance_data.window);

    MG_logic_free(&instance_data.game_data);
    MG_audio_free(&instance_data.audio_data);
	MG_render_free(&instance_data.render_data);
	MG_LL_free(&instance_data.component_list, NULL);

    for (uint32_t i = 0; i < instance_data.shader_count; i++)
		MG_shader_free(instance_data.shader_list[i]);
	for (uint32_t i = 0; i < instance_data.material_count; i++)
		MG_material_free(instance_data.material_list[i]);
    for (uint32_t i = 0; i < instance_data.prefab_count; i++)
		MG_object_free_prefab(instance_data.prefab_list[i]);
	for (uint32_t i = 0; i < instance_data.scene_count; i++)
        MG_scene_free(instance_data.scene_list[i]);

    for (uint32_t i = 0; i < instance_data.model_count; i++)
        MG_model_free(instance_data.model_list[i]);
    for (uint32_t i = 0; i < instance_data.texture_count; i++)
        MG_texture_free(instance_data.texture_list[i]);
    for (uint32_t i = 0; i < instance_data.sound_count; i++)
        MG_sound_free(instance_data.sound_list[i]);

    free(instance_data.shader_list);
    free(instance_data.material_list);
    free(instance_data.prefab_list);
    free(instance_data.scene_list);
    free(instance_data.model_list);
    free(instance_data.texture_list);
	free(instance_data.sound_list);

    instance_data.active = false;
	instance_data.initialized = false;
    MG_INSTANCE = NULL;
}
