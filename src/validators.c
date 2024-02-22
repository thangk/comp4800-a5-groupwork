#include <kth/headers.h>

bool kt_is_valid_number(char *str)
{
    char *endptr;

    // Reset errno to 0 before the call
    errno = 0;
    strtol(str, &endptr, 10);
    if (endptr != str && *endptr == '\0' && errno == 0)
    {
        return true; // It's a valid integer
    }

    // Reset errno to 0 before the call
    errno = 0;
    strtod(str, &endptr);
    if (endptr != str && *endptr == '\0' && errno == 0)
    {
        return true; // It's a valid float/double
    }

    // Not a valid number
    return false;
}

bool kt_is_valid_commandline_input(int argc, char **argv, CmdData **cmd_data)
{
    if (argc == 1)
    {
        return true;
    }

    if (argc != 3)
    {
        g_print("\nInvalid command. Usage: %s <file_name> <frame_number>\n\n", argv[0]);
        return false;
    }

    if (argc == 3)
    {
        g_print("\nReceived command-line argument: %s %s\n\n", argv[1], argv[2]);

        // check if argv[2] is an integer, float or double (only accept those types of numbers)
        bool is_fps_valid_number = kt_is_valid_number(argv[2]);

        if (!is_fps_valid_number)
        {
            g_print("\nError: frame number \"%s\" is not a valid FPS number.\n\n", argv[2]);
            return false;
        }

        // check if the file exists
        if (access(argv[1], F_OK) == -1)
        {
            g_print("\nError: file \"%s\" does not exist.\n\n", argv[1]);
            return false;
        }

        // initialize cmd_data
        (*cmd_data) = malloc(sizeof(CmdData));

        // if all checks pass, set the command-line mode to true and store them to cmd_data
        (*cmd_data)->filename = strdup(argv[1]);
        (*cmd_data)->fps = atof(argv[2]);

        isCommandlineMode = true;

        return true;
    }
}

bool kt_is_fps_too_large(double fps, int total_frames)
{
    fps = kt_is_fps_negative(fps) ? fps * -1 : fps;

    return FIRST_5_FRAMES(fps) >= total_frames;
}

bool kt_is_fps_negative(double fps)
{
    return fps < 0;
}