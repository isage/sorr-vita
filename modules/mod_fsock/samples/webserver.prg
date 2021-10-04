/* fsock example
Mini BennuGD WebServer by SplinterGU
*/
import "fsock";
import "mod_screen";
import "mod_key";
import "mod_timers";
import "mod_text";
import "mod_proc";
import "mod_rand";
import "mod_file";
import "mod_string";
import "mod_wm";
import "mod_time";
import "mod_regex";

GLOBAL
    clients=0;
    logfd;
    DocumentRoot = "webpages";
    AccessLog = "access.log";
End;

PROCESS main();
Begin
	full_screen=false;
	set_title("Mini BennuGD WebServer");
	fsock_init( 0 ); // init fsock library
	write(0,5,5,0,"Clients:");
	write_var(0,5+text_width(0,"Clients:"),5,0,clients);
	server_thread();
end;

function log(string slog)
private
    strint str;
begin
    logfd = fopen(AccessLog, O_RDWR);
    if (!logfd)
        logfd = fopen(AccessLog, O_WRITE);
    end
    fseek(logfd, 0, SEEK_END);
    fputs(logfd,ftime("%d/%m/%Y-%H:%M:%S> ",time()) + slog);
    fclose(logfd);
end

process server_thread()
private
    int socket_listen; // socket_listen to listen to requests
    int connection=0;
    int ipaddr, portaddr;
    int i;
    int informacion; // we'll send the client random numbers between 0 & 32768
begin

    log("BennuGD Web Server, Started!");

    socket_listen=tcpsock_open(); // new socket
    fsock_bind(socket_listen,8080); // we'll listen @ port 8080
    tcpsock_listen(socket_listen,64); // we'll listen for 64 clients

    fsock_fdzero(0);
    fsock_fdset(0,socket_listen);

    while(!key(_ESC))

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
    fp;
    string lang;
begin
    clients++;
	
    //log("Connection from ip "+((ipaddr)&0ffh)+"."+((ipaddr>>8)&0ffh)+"."+
    //((ipaddr>>16)&0ffh)+"."+((ipaddr>>24)&0ffh));
	log("Connection from ip "+ fsock_get_ipstr(&ipaddr) + ":" + portaddr);

    fsock_fdzero(1);
    fsock_fdset(1,sock);

    while(!key(_esc))
    	// As every frame is executed separately, there's no problem with this
        if (fsock_select(1,-1,-1,0)>0 && fsock_fdisset(1,sock))
        	// In the real world, you'd loop here until you got the full package
            rlen=tcpsock_recv(sock,&msg,sizeof(msg));
            if(rlen<=0)
                break;
            end

            // Let's assume we've got the full package
            cnt=split("["+chr(13)+chr(10)+"]+", msg, &hdrFields, sizeof(hdrFields));

            for(n=1;n<cnt;n++)
                if(strcasecmp(substr(hdrFields[n],0,17),"Accept-Language: ")==0)
                    lang=substr(hdrFields[n],17);
                end
            end

            cnt=split("[ ]+", hdrFields[0], &request, sizeof(request));

            log("Request ["+hdrFields[0]+"]");

            if(request[0]!="GET")
                msg=request[2]+" 405 Method Not Allowed";
                tcpsock_send(sock,&msg,len(request[2])+23);
                break;
            end

            /* Expand */

            while((pos=find(request[1],"%")) != -1)
                d1=asc(ucase(substr(request[1], pos+1, 1)));
                d2=asc(ucase(substr(request[1], pos+2, 1)));

                if(d1>=asc("A"))
                    d1-=asc("A")-10;
                else
                    d1-=asc("0");
                end

                if(d2>=asc("A"))
                    d2-=asc("A")-10;
                else
                    d2-=asc("0");
                end

                request[1]=substr(request[1], 0, pos)+chr(d1*16+d2)+substr(request[1], pos+3);
            end

            /* Trying to access a dir above DocumentRoot? */
            if(find(request[1], "..") != -1)
                msg=request[2]+" 403 Forbidden";
                tcpsock_send(sock,&msg,len(request[2])+14);
                break;
            end

            // We can serve GZipped documents!
            if(substr(request[1],-1,1)=="/")
                fp = fopen(DocumentRoot+request[1]+"index.html."+lang, O_READ);
                if (!fp)
                    fp = fopen(DocumentRoot+request[1]+"index.htm."+lang, O_READ);
                end
                fp = fopen(DocumentRoot+request[1]+"index."+lang+".html", O_READ);
                if (!fp)
                    fp = fopen(DocumentRoot+request[1]+"index."+lang+".htm", O_READ);
                end
                if (!fp)
                    fp = fopen(DocumentRoot+request[1]+"index.html", O_READ);
                end
                if (!fp)
                    fp = fopen(DocumentRoot+request[1]+"index.htm", O_READ);
                end
                if (!fp)
                    fp = fopen(DocumentRoot+request[1]+"index.html.html", O_READ);
                end
                if (!fp)
                    fp = fopen(DocumentRoot+request[1]+"index.htm.html", O_READ);
                end
                if (!fp)
                    fp = fopen(DocumentRoot+request[1]+"index.html.htm", O_READ);
                end
                if (!fp)
                    fp = fopen(DocumentRoot+request[1]+"index.htm.htm", O_READ);
                end
            else
                fp = fopen(DocumentRoot+request[1]+"."+lang, O_READ);
                if (!fp)
                    fp = fopen(DocumentRoot+request[1], O_READ);
                end
                if (!fp)
                    fp = fopen(DocumentRoot+request[1]+".html", O_READ);
                end
                if (!fp)
                    fp = fopen(DocumentRoot+request[1]+".htm", O_READ);
                end
            end

            if (!fp)
                msg=request[2]+" 404 Not Found";
                tcpsock_send(sock,&msg,14+len(request[2]));
                break;
            end

            while(!feof(fp))
                slen=fread(fp, msg);
                tcpsock_send(sock,&msg,slen);
                frame;
            end
            fclose(fp);
            break;
        end
        
        fsock_fdset(1,sock); // We must reinclude the socket after the select

        frame;
    end

onexit
	fsock_close(sock); // Close the socket
    clients--;

end
