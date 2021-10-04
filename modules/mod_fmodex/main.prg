import "mod_say"
import "mod_video"
import "mod_key"
import "mod_text"
import "mod_fmodex"
import "mod_file"
import "mod_video"
import "mod_draw"
import "mod_map"
import "mod_math"
import "mod_proc"

Process spectrum_analyser()
Private
	int i=0;
	float width=0.0;

Begin
    // Determine bar width
    width = 640.0/fmodex_spectrumsize;
	
	// Graph used for bar drawing
    graph = map_new(640, 480, 16);
    drawing_map(0, graph);
    drawing_color(rgb(200, 200, 200));
    x = 320; y = 240;
	
	LOOP
		for(i=0; i<fmodex_spectrumsize; i++)
			draw_box(i*width, 480, i*width, 480.0*(1.0-FMODEX_SPECTRUM[i]) );
		end;
        FRAME;
        map_clear(0, graph, rgb(0, 0, 0));
	End;
End;

Process main()
Private
    int channel=0, retval=0, pid=0, i=0;
    string song="";

Begin
    set_mode(640, 480, 16);
    say("Mic section=========================================================");
    retval = fmodex_mic_num();
    if(retval > 0)
        write(0, 320, 240, 4, "Please enter the mic num you want to use with your keyboard");
        // Last microphone should display an error
        for(i=0; i<retval; i++)
            write(0, 320, 250+10*i, 4, i+": "+fmodex_mic_name(i));
        end;
    end;
	
	// Wait for the user to select wich mic they want to use
	while(1)
		// Escape condition
		if(key(_esc))
			return(0);
		end;
		if( (i = ascii-48) >= 0 && (i < retval) )
			break;
		end;
		
		FRAME;
	end;
	
	delete_text(ALL_TEXT);
	
    write(0, 320, 240, 4, "Showing spectrum for mic "+i);
    write(0, 320, 250, 4, "Press enter to continue");

	// Start the spectrum analysis on given mic
    fmodex_mic_get_spectrum(i);

    // Draw spectrum bars
	pid = spectrum_analyser();
	
	// Wait for user to get bored of seeing their own voice dance
    while(! key(_enter))
		FRAME;
    end;

    delete_text(ALL_TEXT);

    fmodex_stop_spectrum();

    say("Sound playback section==============================================");
    if(argc != 2)
        say("ERROR: Must be called as "+argv[0]+" filename");
		unload_map(0, pid.graph);
		signal(pid, S_KILL);
        return -1;
    end;
    if(file_exists(argv[1]))
        song = argv[1];
    else
        say("ERROR: Couldn't find "+argv[0]);
        return -1;
    end;

    // Make sure the song loads correctly
    if((channel = fmodex_song_load(song)) < 0)
        return -1;
    end;

    // Play the song & wait for it to finish
    if(fmodex_song_play(channel) < 0)
        return -1;
    end;
	
	// Start song's spectrum analysis
	fmodex_song_get_spectrum(channel);

    write(0, 320, 240, 4, "Playing on channel "+channel);
    write(0, 320, 250, 4, "Press esc to quit");

    LOOP
        retval = fmodex_song_playing(channel);
        if(retval != 1 || key(_esc))     // Song finished playing
            break;
        end;
        FRAME;
    end;

    // Stop & unload the song
	fmodex_stop_spectrum();
    fmodex_song_stop(channel);
    
	unload_map(0, pid.graph);
	signal(pid, S_KILL);

    say("Bye!");
End;

