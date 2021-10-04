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
    set_mode(640, 480, 32);
	set_title("Mini BennuGD Server");
	fsock_init( 0 ); // init fsock library
	server_thread();
end;

process server_thread()
private
    int socket_listen; // socket_listen to listen to requests
    int connection=0;
    int ipaddr, portaddr;
    int i;
begin
    say("BennuGD Test Server, Started!");

    socket_listen=tcpsock_open(); // new socket
    fsock_bind(socket_listen, 8080); // we'll listen @ port 8080
    tcpsock_listen(socket_listen, 32); // we'll listen for 32 clients

    fsock_fdzero(0);
    fsock_fdset(0,socket_listen);

    while(!key(_esc))
        if (fsock_select(0,-1,-1,0)>0)
            connection=tcpsock_accept(socket_listen, &ipaddr, &portaddr);
            if(connection>0)
                process_client(connection, ipaddr, portaddr);
            end
        end
        
        fsock_fdset (0, socket_listen); // We must reinclude after using select
    	frame;
	end;

onexit
	fsock_quit(); // Now close the fsock lib

end

process process_client(int sock, int ipaddr, int portaddr)
private
    char msg[2048];
    string hdrFields[128];
    string request[3];
    rlen, slen, n, pos, d1, d2, cnt;
begin
    clients++;

	say("Connection from ip "+ fsock_get_ipstr(&ipaddr) + ":" + portaddr);

    fsock_fdzero(1);
    fsock_fdset(1,sock);

    while(focus_status == 1 || ! key(_esc))
    	// As every frame is executed separately, there's no problem with this
        if (fsock_select(1,-1,-1,0)>0 && fsock_fdisset(1,sock))
        	// In the real world, you'd loop here until you got the full package
            rlen=tcpsock_recv(sock,&msg,sizeof(msg));
            if(rlen<=0)
                break;
            end

            say("Got: "+msg);
            // Send the frog color back to the client
            msg = "GREEN";
            tcpsock_send(sock, &msg, sizeof(msg));

            break;
        end
        
        fsock_fdset(1,sock); // We must reinclude the socket after the select

        frame;
    end

onexit
	fsock_close(sock); // Close the socket
    clients--;

end
