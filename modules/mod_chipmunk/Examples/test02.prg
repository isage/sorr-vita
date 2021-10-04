//PRG, ejemplo para el motor de física

import "mod_key";
import "mod_text";
import "mod_proc";
import "mod_grproc";
import "mod_video";
import "mod_wm";
import "mod_map";
import "mod_say";
import "mod_math";
import "mod_screen";
import "mod_rand";
import "mod_chipmunk";

#define alto 700
#define ancho 1280

global
	float u = 0.6;       //fricción para algunos cuerpos

	pelota;              //Gráfico para las pelotas
private
	press;               //Se usa en las pulsaciones de espacio

Begin
/*if (mode_is_ok(1280,800,32,MODE_FULLSCREEN ))            //Si el modo de video está soportado
	set_mode(1280,800,32,MODE_FULLSCREEN );          //Lo define
elseif (mode_is_ok(1280,800,32,0))
	set_mode(1280,800,32);
else
exit("El modo de video no está soportado, intenta cambiarlo");
end
  */
  set_mode(ancho,alto,32);
	pelota=load_png("pelota.png");

    gphysics.interval=1.0/25.0;                   //Velocidad en que el motor está actuando, normalmente se usa la inversa del valor de las imágenes por segundo
    gphysics.SLEEPTIMETHRESHOLD=5;      //Time a group of bodies must remain idle in order to fall asleep
    gphysics.bias_coef=0.3;
    gphysics.gravity_Y=500;
    gphysics.damping=1;
    gphysics.collision_slop=0.5;
    gphysics.iterations=30;

	plano();               //Se pone el piso
	creaPiramide();        //Se crea la pirámide de dominós

	write(0,400,10,4,"Presiona _space");

	while (!key(_esc))
		if (key(_space))
			if (not press)               //si presiona espacio, se arroja una pelota para tumbar los dominós
				cubo();
				press=true;
			end
		else
			press=false;
		end
		frame;

	end
	let_me_alone();
end

process plano()                                      //Crea el plano donde se sienta la pirámide
begin
	lphysics.static=true;                        //Que sea un cuerpo estático (Nada lo mueve excepto tú con x o incr_x)
	lphysics.ShapeType=TYPE_LINE;                //Le agrega un cuerpo tipo línea

	graph=new_map(ancho,20,32);
	map_clear(file,graph,rgb(0,255,0));
	x=ancho/2;	y=alto-10;	z=15;
	loop
		frame;
	end
end


process dominos(x,y,angle)
begin
	graph=new_map(6,40,32);
	map_clear(file,graph,rgb(100,255,255));
    priority=father.priority;
	lphysics.ShapeType=TYPE_box;      //Le crea un cuerpo poligonal, es el más caro computacionalmente hablando, pero es el más exacto para ajustarle una forma
	lphysics.elasticity=0;                     //Define su elasticidad
	lphysics.friction=u;                         //Define su coeficiente de fricción
    forceCreateBody();                 //Se forza la creación del cuerpo
    //Antes de llamar a getOptimalInertia hay que actualizar el cuerpo, ya sea forzando su creación (cuando éste aún no existe) o haciendo un frame(v) antes, con v igual a cualquier valor o sin v.
	lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);
    //sleep(id);      //sleeptimetreshold <>infinityf();
	loop
		frame;

	end
end


//Crea la pirámide, por se código bennu no comentaré nada
function creaPiramide()
private
cpVect offs,pos,aux;
begin

from x=300 to ancho-300 step 40;
    from y=alto-40 to 300 step -46;
        z=dominos(x,y,0);
        sleep(z);
        if (x<ancho-300)
            z=dominos(x+20,y-23,90000);
            sleep(z);
        end
    end
end
end

process cubo()
begin
	x=400;	y=100;
        graph=pelota;

	lphysics.mass=10;
	lphysics.inertia= cpMomentForCircle(10.0, 0.0, 10.0, 0.0,0.0);
	lphysics.ShapeType=TYPE_circle;
	lphysics.elasticity=0.1;
	lphysics.friction=0.6;

        FORCECREATEBODY(); //forza la creación del cuerpo físico inmediatamente para comenzar a usarlo
        Slew( rand(300,900),500, 1);  //Mueve la pelóta hacia el punto ( rand[300,900] ,500)
	loop
            if (out_region(id,0))
    			signal(id,s_kill);
    		end
		frame;

	end
end
