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
import "mod_chipmunk"
import "mod_math"


global
        //gráficos
	cuadro;
	bola;
	bomba;

Begin
	set_mode(800,600,32);

	bola=map_new(20,20,32);
	map_clear(0,bola,0FFFFFF00h);
 	gphysics.damping=0.8;                           //Viscosidad entre 0 y 1

	//Cantidad de correcció de intersección cada frame/phresolution
        gphysics.bias_coef=0;


	bomba(250,150,10.0);
        avd();
while (!key(_esc))

		frame;

	end
	let_me_alone();
end
process bomba(x,y,float masa)
private
	cpVect pos;
	pego=true;           //se considera que la bomba inicia tocando al protagonista
cpVect	velocidad;

begin

        lphysics.mass=masa;
	priority=father.priority-2;
	lphysics.group=2;
	lphysics.elasticity=0.3;
	lphysics.friction=0;
	graph=bola;
	lphysics.ShapeType=TYPE_CIRCLE;
	forceCreateBody();
	//Antes de llamar a getOptimalInertia hay que actualizar el cuerpo, ya sea forzando su creación (cuando éste aún no existe) o haciendo un frame(v) antes, con v igual a cualquier valor o sin v.
        lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);
        write_var(0,400,300,4,velocidad.x);
        write_var(0,400,350,4,velocidad.y);
        APPLYIMPULSE(id,1000,100,0,0);
	while (!key(_esc))
	getBody(lphysics.body,CP_C_V,&velocidad);
		frame;

	end
end

process avd()
private int av=-90000;
begin
  x=100;
  y=100;
  graph=bola;
  loop
    if (av<0)
      av+=300;
      x-=sin(av)*4;
    end
    frame;
  end
end

