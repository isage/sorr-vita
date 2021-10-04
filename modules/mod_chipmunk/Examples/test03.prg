//genera cuerpos a partir de lista de vértices
//Para una descripción más detallada de las partes que no se comentan, revisar los test anteriores.

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


global
	triangulo;
	tot=0;
private
	press;
	float distancia;
	bl;
Begin
	set_mode(800,600,32);
//	set_fps(0,0);
	write_var(0,400,100,4,fps);
	triangulo=load_png("tri.png");
	gphysics.gravity_Y=300;
        gphysics.bias_coef=0;
	for (z=0;z<6;z++)
		plano();                                          //Crea los planos donde caen los triángulos
	end
	write(0,400,10,4,"Mueve los triangulos con _left, _right ");
        triangulo();
	while (!key(_esc))
		if (rand(0,30)==3 and tot<30)                    //Genera aleatoriamente los triángulos.
			triangulo();
		end
		frame;

	end
	let_me_alone();
end

process plano()
private

begin

	lphysics.static=true;
	lphysics.elasticity=1;
	lphysics.friction=0.4;
	lphysics.group=3;                                        //Pone a todos los planos en el mismo grupo para que no generen colsión entre sí
	lphysics.ShapeType=TYPE_LINE;

	graph=new_map(rand(200,400),20,32);
	x=rand(100,700);	y=rand(300,550);	z=15;
	angle=rand(0,360000);
	map_clear(file,graph,rgb(0,255,0));
	loop
		frame;
	end
end

process triangulo()
private
	id3;
	datos[]=0,0,3,    //centro (x,y), cantidad de puntos
	-21,-24,          //punto 1
	-21,24,           //punto 2
        21,0;
begin
	tot++;
	x=400;
	y=100;

        graph=triangulo;
	priority=father.priority-2;

	lphysics.params=&datos;                  //Crea el cuerpo a partir de un arreglo con los vértices. Véase la ayuda para más información
	lphysics.mass=10;
	lphysics.ELASTICITY=0.1;
	lphysics.friction=0.3;
	lphysics.ShapeType=TYPE_CONVEX_POLYGON;
	forceCreateBody();                 //calcula el cuerpo a través de un arreglo con los vértices

	//Antes de llamar a getOptimalInertia hay que actualizar el cuerpo, ya sea forzando su creación (cuando éste aún no existe) o haciendo un frame(v) antes, con v igual a cualquier valor o sin v.
        lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);

	while (!key(_esc))
		if (key(_right))
			lphysics.incr_x+=5;                                   //Mueve el objeto en x
		end

		if (key(_left))
			lphysics.incr_x-=5;
		end
		if (out_region(id,0))
			signal(id,s_kill);
		end
		frame;

	end
	onexit;
	tot--;
end
