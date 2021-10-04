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

global
        //gráficos
	cuadro;
	bola;
	bomba;

Begin
	set_mode(800,600,32);
	cuadro=load_png("cuadro.png");
	bola=load_png("bola40.png");
	bomba=load_png("bomba.png");
 	gphysics.damping=1;                           //Viscosidad entre 0 y 1


        //Se genera el mapa
 	for (z=0;z<15;z++)
		cuadro(z*50+50,50);
	end
	for (z=0;z<15;z++)
		cuadro(z*50+50,550);
	end
	for (y=0;y<4;y++)
		for (z=0;z<6;z++)
			cuadro(z*100+150,y*100+150);
		end
	end

	for (z=0;z<9;z++)
		cuadro(50,z*50+100);
	end

	for (z=0;z<9;z++)
		cuadro(750,z*50+100);
	end

        //Se genera el personaje
	bomberman(150,150);

	//Cantidad de correcció de intersección cada frame/phresolution
        gphysics.bias_coef=0;

	write(0,400,10,4,"Presiona v para dejar una bomba que se empuja");
	write(0,400,20,4,"Presiona space para dejar una bomba que NO se empuja");
while (!key(_esc))

		frame;

	end
	let_me_alone();
end

process cuadro(x,y)
begin
	lphysics.static=true;            //El objeto es estático (No se mueve por interacción con gravedad u otros objetos)
	lphysics.mass=infinityf();       //Todo objeto estático debe tener su masa e inercia igual a infinito
	lphysics.inertia=infinityf();
	lphysics.elasticity=1;           //La elasticidad va, preferentemente, de 0 a 1, y se refiere a qué tanto el objeto rebotará al contacto con otros
	lphysics.friction=1;             //La fricción va, preferentemente, de 0 a 1, y se refiere a cuánta velocidad perderá el objeto al friccionar. Una friccón de cero hará que el objeto no gire al tocar los otros objetos
	lphysics.group=1;                //Grupo al que pertenece el proceso. Procesos del mismo grupo no colisionan
	lphysics.ShapeType=TYPE_box;  //Tipo de cuerpo, esto inicializa la creación del cuerpo
	graph=cuadro;
	z=-15;
	loop
		frame;
	end
end
      #define vel 8
process bomberman(x,y)
private
	cpVect pos;
	press,press1;
	id2;
begin
	x=400;
	y=100;
	lphysics.mass=10;                                                     //Masa del cuerpo
	lphysics.inertia=cpMomentForCircle(lphysics.mass,0.0,40.0,0.0,0.0);   //Se genera el valor más apropiado para la inercia de este objeto
	priority=father.priority-2;
	lphysics.elasticity=0;
	lphysics.friction=0;                 //para que no gire al friccionar con otros objetos
	lphysics.group=2;
	lphysics.ShapeType=TYPE_CIRCLE;      //El forma del cuerpo es circular, es la forma más económica en cuanto a recursos se trata
	graph=bola;
	while (!key(_esc))
		if (key(_right))
			lphysics.incr_x=vel; //Mueve la pelota en x
                        angle=0;
		end

		if (key(_left))
			lphysics.incr_x=-vel;
			angle=180000;
		end

		if (key(_up))
			lphysics.incr_y=-vel; //Mueve la pelota en y
			angle=90000;
		end

		if (key(_down))
			lphysics.incr_y=vel;
			angle=270000;
		end

		if (key(_space))
			if (press)
				bomba(x,y,20000.0);
				press=false;
			end
		else
			press=true;
		end

		if (key(_v))
			if (press1)
				bomba(x,y,1.0);
				press1=false;
			end
		else
			press1=true;
		end

		frame;

	end
end

process bomba(x,y,float masa)
private
	cpVect pos;
	pego=true;           //se considera que la bomba inicia tocando al protagonista

begin
        lphysics.mass=masa;
	priority=father.priority-2;
	lphysics.group=2;
	lphysics.elasticity=0.3;
	lphysics.friction=0;
	graph=bomba;
	lphysics.ShapeType=TYPE_CIRCLE;
	forceCreateBody();
	//Antes de llamar a getOptimalInertia hay que actualizar el cuerpo, ya sea forzando su creación (cuando éste aún no existe) o haciendo un frame(v) antes, con v igual a cualquier valor o sin v.
        lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);
	while (!key(_esc))
		if (pego and not collision(father))            //si ya no está tocando al protagonista y antes lo tocaba
			lphysics.group=CP_NO_GROUP;            //cambia el grupo al que pertenece la bomba, de esta forma el protagonista ya no lo traspasará
			pego=false;
		end
		frame;

	end
end

