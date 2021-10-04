/* fsock example
Mini BennuGD WebServer by SplinterGU
*/
import "fsock"
import "mod_screen"
import "mod_video"
import "mod_map"
import "mod_key"
import "mod_timers"
import "mod_text"
import "mod_proc"
import "mod_rand"
import "mod_file"
import "mod_string"
import "mod_wm"
import "mod_time"
import "mod_regex"
import "mod_say"
import "mod_multi"
import "mod_mouse"

GLOBAL
    clients=0;
    width = 0;
    height = 0;
End;

PROCESS main();
Begin
    // Get the real screen resolution we're running at
    width = graphic_info(0, 0, G_WIDTH);
    height = graphic_info(0, 0, G_HEIGHT);
    set_mode(width, height, 32);
	set_title("Mini BennuGD Client");
	fsock_init( 0 ); // init fsock library
    write(0, 0, 0, 0, "Touch the screen to send a message to the server");
    while(focus_status == 1)
        if(multi_info(0, "ACTIVE") > 0)
            client();
        end
        FRAME;
    end;
    
    say("Quitting!");
end;

process client()
private
    int socket, rlen; // socket_listen to listen to requests
    char dat[13]="Hello, World!";
    char color[10];
begin

    socket=tcpsock_open(); // new socket
    if ( tcpsock_connect(socket, "192.168.0.194", "8080") != 0 )
        say("Error connecting to the server");
        return;
    end

    //say("Sending: "+dat);
    tcpsock_send(socket, &dat, len(dat));
    
    // Wait for the server's answer
    while(focus_status == 1)
      	// In the real world, you'd loop here until you got the full package
        rlen=tcpsock_recv(socket, &color, sizeof(color));
        if(rlen>0)
            say("Your frog will be: "+color);
            break;
        end
        FRAME;
    end

onexit
	fsock_quit(); // Now close the fsock lib
end
