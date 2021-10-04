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
import "mod_chipmunk";
import "mod_draw";
import "mod_rand";

#include "jkeys.lib"


global
	pelota,barra,barraND;
	mass=1,inerty;
private
	press;
	float distancia;
	bl;
	cpVect l[]=-30.0,-5.0,
        -30.0,5.0,
        30.0,5.0,
        30.0,-5.0;
Begin
	set_mode(320,240,8);
	set_fps(25,0);
	jkeys_set_default_keys();
	jkeys_init();
	pelota=new_map(15,15,8);
	drawing_map(0,pelota);
	drawing_color(rgb(255,0,0));
	draw_fcircle(7,7,7);
	
        gphysics.gravity_Y=70;
	gphysics.phresolution=1;                                      //Realiza una iteración por frame, del motor, para ahorrar recursos.
	inerty=cpMomentForSegment(mass,-30.0,0.0,30.0,0.0);

        barra=new_map(60,10,8);
        barraND=new_map(60,10,8);
	map_clear(0,barra,rgb(50,200,20));
	map_clear(0,barraND,rgb(180,0,130));


        addInanimateShape(type_line, 0.0  ,0.0,  0.0,240.0,0.0);
        addInanimateShape(type_line, 319.0,0.0,319.0,240.0,0.0);
        addInanimateShape(type_line, 0.0  ,0.0,319.0,  0.0,0.0);

        creaZona();

	write(0,10,10,0,"Deja caer la pelota al agua");

        graph=new_map(320,10,8);
        map_clear(file,graph,rgb(0,0,255));
        x=160;
        y=235;
        mouse.graph=pelota;
	while (!jkeys_state[_JKEY_MENU])
		frame;

	end
        jkeys_exit();
	let_me_alone();

end

function creaZona();
begin
signal(type plano,s_kill);
        bol();
        plano(160,28,barra);

        z=plano(140,50,barraND);
       	addPivotJoint(z,0,110,50);                      //agrega un pivote entre el plano y el fondo
	addDampedRotarySpring(z, 0, 0,95000,2);         //Agrega un resorte entre el plano y el fondo

        z=plano(200,90,barraND);
       	addPivotJoint(z,0,230,90);                      //Agrega un pivote entre el plano y el fondo
        addSimpleMotor(z,0,80000);                      //Agrega un motor al plano

	z=plano(58,135,barraND);
       	addPinJoint(z,0,0,0,150,120);                   //Agrega una unión entre el plano y el fondo para simular un movimiento estilo péndulo

       	plano(160,165,barra);

end

process plano(x,y,graph)
private

begin
        if (graph==barra)
	 lphysics.static=true;
	 lphysics.mass=infinityf();
	 lphysics.inertia=infinityf();
	else
	lphysics.static=false;
        lphysics.mass=mass;
	lphysics.inertia=inerty;
	end

	lphysics.elasticity=1;
	lphysics.friction=1;
	lphysics.ShapeType=TYPE_line;
	FORCECREATEBODY();
	loop
	if (mouse.left and graph==barra and collision(type mouse))
	   signal(id,s_kill);
	end
		frame;
	end
end

process bol()
begin
        x=160;
	y=20;
	lphysics.static=false;
	lphysics.mass=10;
	lphysics.inertia=CPMOMENTFORCIRCLE(lphysics.mass,0.0,10.0,0.0,0.0);
	lphysics.ELASTICITY=0.1;
	lphysics.friction=0.9;
	lphysics.collisionType=1;
	lphysics.ShapeType=TYPE_CIRCLE;
	graph=pelota;
        FORCECREATEBODY();
	loop
		if (out_region(id,0))
                        z=write(0,100,100,4,"Felicidades!!, reiniciando...");
                        frame(4500);
                        delete_text(z);
			creaZona();
			signal(id,s_kill);
		end
		frame;

	end
end
