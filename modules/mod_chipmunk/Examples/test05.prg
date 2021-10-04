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


global
	pelota;
	cpVect point,ot;
	handler;
private
	press;
	cpSegmentQueryInfo info ;
	cpContactPointSet Ct;
	float distancia;
	bl;
Begin
        set_mode(800,600,32);
	pelota=load_png("bola40.png");

        gphysics.gravity_Y=20;
	gphysics.bias_coef=0;              //Cualquier traslape debe ser corregido inmediatamente. En el caso de apilamientos, la pila puede caerse más fácilmente.
	handler=COLLISIONHANDLERNEW(1,2);    //detecta donde colisionan los objetos con collisionType 1 y 2

        plano();
	for (z=0;z<6;z++)
		cubo();
	end

	write(0,400,10,4,"Mueve la pelota con _left, _right y gira el plano con _a, _d");
	write(0,400,30,4,"Usa espacio para crear más círculos. ");
	mouse.graph=pelota;
	mouse.size=50;
	write_var(0,400,300,4,distancia);

	while (!key(_esc))
		bl=SpaceSegmentQueryFirst( 400,300, mouse.x,mouse.y, CP_ALL_LAYERS, CP_NO_GROUP, &info);  //busca el objeto que priemro colisione con una linea formade del punto (x,y) al puntu (x1,y1)
		x=mouse.x;
		y=mouse.y;
		if(mouse.x<800 and mouse.y<600 and bl!=-1)                                       //si encontró colisión
			SegmentQueryHitPoint(400,300, mouse.x,mouse.y, &info,&x,&y);             //busca el punto de colisión y lo almacena en x,y
			distancia=SegmentQueryHitDist(400,300, mouse.x,mouse.y, &info);          //encuentra la distancia entre el inicio de la linea y la colisión
		end

		delete_draw(0);
		drawing_Z(-100);
		draw_line(400,300,x,y);                                                          //imprime una linea desde el comienzo de la linea de colisión hasta el mouse o el punto de colisión
		while (GETCOLLISIONINFO(handler,&ct))
			for (z=0;z<ct.count;z++)
				draw_fcircle(ct.points[z].Point.x,ct.points[z].Point.y,5);
			end
		end
                
		if (key(_space))
			if (not press)
				cubo();                                                          //Si presionas espacio, Genera más esferas
				press=true;
			end
		else
			press=false;
		end
		frame;

	end

	let_me_alone();

end

process plano()
begin

        lphysics.static=true;
        lphysics.collisionType=0;          //Tipo de colisión 2, para el uso del handler
	lphysics.friction=0.4;
        lphysics.ShapeType=TYPE_LINE;
	graph=new_map(500,20,32);
    set_center(file,graph,0,0);
	x=400;	y=440;	z=15;	angle=-45000;
	map_clear(file,graph,rgb(0,255,0));
	write_var(0,200,400,4,angle);
	loop
		if (key(_a))
			angle+=1000;
		end
		if (key(_d))
			angle-=1000;
		end
		frame;
	end
end

process cubo()
private
id3;
begin
	graph=pelota;   x=400;	y=100;
        priority=father.priority-2;

	lphysics.mass=10;
	lphysics.ELASTICITY=0.1;
	lphysics.friction=0.3;
	lphysics.collisionType=1;
	lphysics.ShapeType=TYPE_CIRCLE;


        forceCreateBody();
	lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);

	while (id3=GETCONSTRAINTS(id))       //no tienen constraints, no debe salir en consola nada
	say("const"+id3);
	end
	while (id3=GETSHAPES(id))
	say("shapes"+id3);                   //sólo tienen un shape, así que sólo debe haber 6 shapes en la consola al principio.
	end

	while (!key(_esc))
		if (key(_right))
			lphysics.incr_x+=5;
		end

		if (key(_left))
			lphysics.incr_x-=5;
		end
		if (out_region(id,0))
			signal(id,s_kill);
		end
		frame;

	end
end
