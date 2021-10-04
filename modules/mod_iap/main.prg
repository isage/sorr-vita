import "mod_video"
import "mod_iap"
import "mod_say"
import "mod_mouse"
import "mod_text"
import "mod_map"
import "mod_file"

Global
int width = 800;
int height = 600;
int quit=0;
end;

Process main()
Private
    int i=0, status=0, curl=0;
    int tostring=0;     // Switch to 1 to download to a string
    string output;

Begin
    while(! mouse.right)
        FRAME;
    end;
    quit=1;
End;
