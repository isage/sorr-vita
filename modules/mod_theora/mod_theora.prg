import "mod_key"
import "mod_video"
import "mod_mouse"
import "mod_say"
import "mod_map"
import "mod_file"
import "mod_proc"
import "mod_timers"
import "mod_wm"
import "mod_theora"
import "mod_sound"

/* Player main window */
Process main()
Private
    string fname="";
    int time=0, delay=30, track=1;
    int resx=640, resy=480;
    int song;

Begin
    // Handle the command line
    if(argc != 2)
        say("Must be given file to play, quitting.");
        exit();
    end;

    // Try to find the file that the user wants us to play, or die
    fname = argv[1];
    if(! fexists(fname))
        say("Couldn't find "+fname+" to be played, quitting.");
        exit();
    end

    /* Start the graphics subsystem */
    resx=640; resy=480;
    set_mode(resx, resy, 16, MODE_WINDOW);
    
    song = load_song("1.ogg");
    play_song(song, 0);

    if((graph = video_play(fname)) == -1)
        say("Sorry, I couldn't play your video :(");
        exit();
    end;

    x = resx/2; y = resy/2;

    while(! key(_esc))
        if(timer[0] > time+delay)
            if(key(_right))
                angle += 1000;
                time = timer[0];
            end;
            if(key(_left))
                angle -= 1000;
                time = timer[0];
            end;
        end;
        FRAME;
    end;

    video_stop();

    while(! key(_q))
        // If you press space bar, we start over
        if(key(_space))
            main();
            return;
        end;
        FRAME;
    end;
    
    unload_song(song);
    
    exit();
End;
