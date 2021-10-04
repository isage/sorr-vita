//Calcula los cuerpos convexos de las figuras automáticamente.

/*La creación automática de cuerpos usa la variable global gphysics.distPercent para
la optimización del cuerpo. Esta variable selecciona el rango que habrá entre dos vértices consecutivos 
para que sean considerado como vértices de una misma línea (y sea eliminado uno de ellos, optimizando el cuerpo).
Por defecto tiene asignado el valor de 3000 (3 grados).

*/
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
	triangulo,exa;
	tot=0;
	cpVect verticesTriangulo[5];
private
	press;
	float distancia;
	bl;
Begin
	set_mode(800,600,32);
//	set_fps(0,0);
	write_var(0,400,100,4,fps);
	triangulo=load_png("tri.png");
	exa=load_png("exa.png");
	gphysics.gravity_Y=300;
        gphysics.bias_coef=0;
        gphysics.distPercent=3;
        say(&verticesTriangulo);
        say(x=CALCULATECONVEXBODY(0,triangulo,&verticesTriangulo));       //calcula los vértices del cuerpo convexo del gráfico triángulo (file, graph, arreglo para rellenar) y retorna la cantidad de vértices que calculó.
        //en test test no es necesario usar esta función, la creación de los cuerpos se hace automáticamente en el proceso  triángulo al no haber nada
        //asignado a la variable local lphysics.params, sin embargo se usa para mostrar los vértices que se calculan para el cuerpo.
        //Se recomienda que el arreglo que se le pase a la función CALCULATECONVEXBODY sea del perímetro del gráfico (2*ancho+ 2*alto) para que no falte memoria al cualcular el cuerpo y almacenar los vértices.
        for (z=0;z<x;z++)
            say("vertice = "+verticesTriangulo[z].x+" "+verticesTriangulo[z].y);
        end

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
    x1,y1;
begin
	tot++;
	x=400;
	y=100;
	if (rand(0,10)>4)
	   graph=exa;
	else
	   graph=triangulo;
	end
    size=rand(10,140);
	priority=father.priority-2;
	lphysics.mass=10;
	lphysics.ELASTICITY=0.1;
	lphysics.friction=0.3;
	lphysics.ShapeType=TYPE_CONVEX_POLYGON;
	forceCreateBody();
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
