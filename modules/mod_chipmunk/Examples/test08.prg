
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
import "mod_draw";
import "mod_scroll";
import "mod_chipmunk"
#define ancho 0
#define alto 0
global
	int num = 4;               //Número de vértices del carro
	cpVect verts[] =           //Lista con vértices del cuerpo del carro
	-40,-15,
	-40, 15,
	40, 15,
	40,-15;
	//gráficos para la primera parte, las pelotas
	pelota;

	handler;                  //Handler para colisión del protagonista de la segunda parte

	//gráficos y fpg para la segunda parte
	fondo;
	fpg;

private

Begin
	//full_screen=true;
	set_mode(800,600,32);

	gphysics.gravity_y=100;          //gravedad en y
	pelota=load_png("pelota.png");
	gphysics.interval=1.0/25.0;

	gphysics.bias_coef=0.3;
	
        mouse.graph=pelota;
	armaMapa();
	colocaObjetos();
	mouseP();
	
        while (!key(_esc))


		frame;
	end
	delete_draw(0);
	delete_text(0);
	let_me_alone();
        cleanSpace();      //Elimina todos los cuerpos físicos. Es necesario llamar con anterioridad let_me_alone()
	inicia();          //Inicia la zona del player jump
end

process mouseP()
private
	mouseJoint;
	x0,y0;
begin
 	lphysics.static=true;
	priority=father.priority;
	lphysics.ShapeType=TYPE_empty;
	graph=pelota;
	FORCECREATEBODY();
	loop
		x=mouse.x;
		y=mouse.y;
		 if (mouse.left)    //permite mover los procesos con el ratón
			z = SpacePointQueryFirst( x, y, GRABABLE_MASK_BIT, CP_NO_GROUP);
			if (z and mouseJoint==0)
				WORLD2LOCAL(z, x, y,&x0, &y0);
				mouseJoint=addPivotJoint2(id, z, 0,0, x0,y0);
				DefConstraintF(mouseJoint,CP_C_MAXFORCE,50000);
				DefConstraintF(mouseJoint,CP_C_BIASCOEF,0.15);
			end
		elseif (mouseJoint<>0)
			RemoveConstraint(mouseJoint);
			mouseJoint = 0;
		end
		frame;
	end
end

process bola(x,y)
private
	esferaShape1;
	cpvect a;
begin
	graph=pelota;
	priority=father.priority;

	lphysics.inertia= cpMomentForCircle(1, 0.0, 15, &a);
	lphysics.ShapeType=TYPE_circle;
	lphysics.elasticity=0.3;
	lphysics.friction=0.7;
	FORCECREATEBODY(); //forza la creación del cuerpo físico inmediatamente para comenzar a usarlo
	loop
		frame;

	end
end

process addlever(x,y)
private
	esferaShape1;
	cpvect a,b;
begin
	priority=father.priority;
	graph=new_map(20,60,32);
	map_clear(file,graph,rgb(5,30,250));

	lphysics.inertia=cpMomentForSegment(lphysics.mass, 0.0,15.0,0.0,-15.0);
        lphysics.ShapeType=TYPE_line;
	lphysics.elasticity=0.1;
	lphysics.friction=0.7;
	FORCECREATEBODY(); //forza la creación del cuerpo físico inmediatamente para comenzar a usarlo
	loop
		frame;

	end
end


process addbar(x,y)
private
	esferaShape1;
	cpvect a,b;
begin
	priority=father.priority;
	graph=new_map(10,60,32);
	map_clear(file,graph,rgb(5,30,250));

	lphysics.inertia=cpMomentForSegment(1, 0.0,30.0, 0.0,-30.0);
	lphysics.ShapeType=TYPE_line;
	lphysics.elasticity=0.3;
	lphysics.friction=0.7;
	FORCECREATEBODY(); //forza la creación del cuerpo físico inmediatamente para comenzar a usarlo
	loop
		frame;

	end
end

process addwheel(x,y)
private
	esferaShape1;
begin
	priority=father.priority;
	graph=pelota;
	size=150;

	lphysics.inertia=cpMomentForCircle(1.0,0,15.0,0.0,0.0);
	lphysics.shape = addCircleShape(0,0,15);//también se pueden crear por las funciones apropiadas
	lphysics.elasticity=0.001;
	lphysics.friction=0.7;
	lphysics.group=1;

	loop
		frame;

	end
end

process addchassis(x,y)
begin
	lphysics.mass=5;
	lphysics.inertia=cpMomentForPoly(5.0, num, &verts, 0.0,0.0);     //Encuentra la inercia adecuada para el polígono formado por los vértices de la lista verts
	priority=father.priority;
	lphysics.ShapeType=TYPE_box;
	lphysics.elasticity=0.1;
	lphysics.friction=0.7;
	lphysics.group=1;
	graph=new_map(80,30,32);
	map_clear(file,graph,rgb(100,100,255));
        FORCECREATEBODY(); //forza la creación del cuerpo físico inmediatamente para comenzar a usarlo
	while (!key(_esc))

		frame;

	end
end



//Crea la escena primera.
function colocaObjetos();
private
	body1, body2;
begin

        //Para una explicación referente a cada join, revisar la ayuda de la página 26 - 30
	x=3; y=3;
	body1 = bola(x*200+70,y*150+60);
	body2 = bola(x*200+140,y*150+120);
	addPinJoint(body1, body2,15,0,-15,0);


	x=2; y=3;
	body1 = bola(x*200+70,y*150+60);
	body2 = bola(x*200+140,y*150+120);
	addSlideJoint(body1, body2, 15,0,-15,0,20,40);

	x=1; y=3;
	body1 = bola(x*200+70,y*150+60);
	body2 = bola(x*200+140,y*150+120);
	addPivotJoint(body1, body2,x*200+100,y*150+30);

	x=0; y=3;
	body1 = bola(x*200+70,y*150+60);
	body2 = bola(x*200+140,y*150+120);
	addGrooveJoint(body1, body2, 30,30,30,-30,-30,0);

	x=3; y=2;
	body1 = bola(x*200+70,y*150+70);
	body2 = bola(x*200+140,y*150+70);
	addDampedSpring(body1, body2, 15,0,-15,0,20.0,5.0,0.3);
	x=2; y=2;
	body1 = addbar(x*200+70,y*150+60);
	body2 = addbar(x*200+140,y*150+60);
	addPivotJoint(body1,0,x*200+70,y*150+60);
	addPivotJoint(body2,0,x*200+140,y*150+60);
	addDampedRotarySpring(body1, body2, 0,3000,60);
	write(0,x*200+70,y*150+60,4,"addDampedRotarySpring");

	x=1; y=2;
	body1 = addLever(x*200+70,y*150+60);
	body2 = addLever(x*200+140,y*150+60);
	addPivotJoint(body1,0,x*200+70,y*150+60);
	addPivotJoint(body2,0,x*200+140,y*150+60);
	addRotaryLimitJoint(body1, body2, -90000,90000);
	write(0,x*200+70,y*150+60,4,"addRotaryLimitJoint");

	x=0; y=2;
	body1 = addLever(x*200+70,y*150+60);
	body2 = addLever(x*200+150,y*150+60);
	addPivotJoint(body1,0,x*200+70,y*150+60);
	addPivotJoint(body2,0,x*200+140,y*150+60);
	addRatchetJoint(body1, body2, 0,90000);
	write(0,x*200+70,y*150+60,4,"cpRatchetJointNew");

	x=3; y=1;
	body1 = addBar(x*200+70,y*150+60);
	body2 = addBar(x*200+140,y*150+60);
	addPivotJoint(body1,0,x*200+70,y*150+60);
	addPivotJoint(body2,0,x*200+140,y*150+60);
	addRatchetJoint(body1, body2, 0,2);
	write(0,x*200+70,y*150+60,4,"cpRatchetJointNew");

	x=2; y=1;
	body1 = addBar(x*200+70,y*150+60);
	body2 = addBar(x*200+140,y*150+60);
	addPivotJoint(body1,0,x*200+70,y*150+60);
	addPivotJoint(body2,0,x*200+140,y*150+60);
	addSimpleMotor(body1, body2, 180000);
	write(0,x*200+70,y*150+60,4,"addSimpleMotor");

	x=1; y=1;
	body1 = addWheel(50,100);
	body2 = addWheel(110,100);
	z=      addChassis(80,60);
	addGrooveJoint(z,body1,-30,10,-30,40,0,0);
	addGrooveJoint(z,body2,30,10,30,40,0,0);

	addDampedSpring(z, body1,-30,0,0,0,50,20,1.5);
	addDampedSpring(z, body2,30,0,0,0,50,20,1.5);

end

//Arma las líneas del mapa
function armaMapa()
private
	cpvect a,b;
	float radio=1;
begin


	cpv(1,0,&a);
	cpv(1,600,&b);
	draw_line(a.x,a.y,b.x,b.y);
	lphysics.shape = addInanimateShape(type_line, a.x,a.y,b.x,b.y, radio);      //Se crean procesos que se agregan al fondo. para eliminarlos se debe eliminar toda la zona con la función cleanSpace();
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);

	cpv(200,0,&a);                               cpv(200,600,&b);
	draw_line(a.x,a.y,b.x,b.y);
	lphysics.shape = addInanimateShape(type_line, a.x,a.y,b.x,b.y, radio);
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);

	cpv(400,0,&a);                               cpv(400,600,&b);
	draw_line(a.x,a.y,b.x,b.y);
	lphysics.shape = addInanimateShape(type_line, a.x,a.y,b.x,b.y, radio);
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);

	cpv(600,0,&a);                               cpv(600,600,&b);
	draw_line(a.x,a.y,b.x,b.y);
	lphysics.shape = addInanimateShape(type_line, a.x,a.y,b.x,b.y, radio);
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);

	cpv(799,0,&a);                               cpv(799,600,&b);
	draw_line(a.x,a.y,b.x,b.y);
	lphysics.shape = addInanimateShape(type_line, a.x,a.y,b.x,b.y, radio);
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);

	cpv(1,1,&a);                               cpv(800,1,&b);
	draw_line(a.x,a.y,b.x,b.y);
	lphysics.shape = addInanimateShape(type_line, a.x,a.y,b.x,b.y, radio);
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);

	cpv(1,150,&a);                               cpv(800,150,&b);
	draw_line(a.x,a.y,b.x,b.y);
	lphysics.shape = addInanimateShape(type_line, a.x,a.y,b.x,b.y, radio);
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);

	cpv(1,300,&a);                               cpv(800,300,&b);
	draw_line(a.x,a.y,b.x,b.y);
	lphysics.shape = addInanimateShape(type_line, a.x,a.y,b.x,b.y, radio);
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);

	cpv(1,450,&a);                               cpv(800,450,&b);
	draw_line(a.x,a.y,b.x,b.y);
	lphysics.shape = addInanimateShape(type_line, a.x,a.y,b.x,b.y, radio);
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);

	cpv(1,599,&a);                               cpv(800,599,&b);
	draw_line(a.x,a.y,b.x,b.y);
	lphysics.shape = addInanimateShape(type_line, a.x,a.y,b.x,b.y, radio);
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);

end


//Comienza la parte de la zona del juego de plataformas
process inicia()
private
	press;
	cpSegmentQueryInfo info ;
	float distancia;
	bl;
Begin
        while(key(_esc))
            frame;
        end

	fondo=load_png("fondoImg.png");
	fpg=load_fpg("graficos.fpg");
	start_scroll(0,0,fondo,0,0,0);
	gphysics.gravity_Y=300;
	gphysics.damping=0.9;
	gphysics.interval=1.0/25.0;
	handler=COLLISIONHANDLERNEW(1,2);    //detecta donde colisionan los objetos con collisionType 1 y 2
	plano(400,440,0);
	plano(700,340,6000);
	plano(1000,640,0);
	plano(1300,740,-10000);
	cubo(400,400);
	gphysics.bias_coef=0;
	mouse.graph=pelota;
	mouse.size=50;
        frame;
	while (!key(_esc))
        	frame;
	end

	let_me_alone();
	//	Ch_End();

end

process plano(x,y,angle)
private
//	datos[]=-250,0,250,0,50;
begin
	lphysics.static=true;
	lphysics.mass=infinityf();
	lphysics.inertia=infinityf();
	lphysics.collisionType=2;
	lphysics.elasticity=1;
	lphysics.friction=0.4;
	graph=new_map(300,20,32);
	z=15;
	lphysics.ShapeType=TYPE_LINE;
	map_clear(file,graph,rgb(0,255,0));
        ctype=c_scroll;
	loop
		frame;
	end
end

process cubo(x,y)
private
	id3;
	cpvect nPiso,vel,inter,aux,pos=0,-17;
	cpContactPointSet Ct;
	int x0,y0;
        sub;
        dat[]=0,0,20;
begin
	lphysics.static=false;
	lphysics.mass=10;
	lphysics.inertia= cpMomentForCircle(lphysics.mass, 0, 10, &cpvzero);
	lphysics.params=&dat;                     //parámetros para la creación del lphysics.shape: centro 0,0 y 20 de radio.
	for (z=302;z<310;z++)
            set_center(fpg,z,17,22);
        end
	file=fpg;
        graph=302;
	lphysics.elasticity=0;
	lphysics.friction=0;
	lphysics.collisionType=1;
	priority=father.priority-2;
	lphysics.ShapeType=TYPE_circle;
	FORCECREATEBODY();
	ctype=c_scroll;
        scroll.camera=id;
	loop


		if (key(_right))
		        GetBody(lphysics.body,cp_c_v,&vel);
			cpvperp(&nPiso,&aux);
			cpvmult(&aux, 10,&inter);
                        cpvadd(&vel, &inter,&inter);
                        inter.x=cpfclamp(inter.x, -120, 120);
                        DefBodyP(lphysics.body,CP_C_V,&inter);
                        graph++;
                        flags=1;

		end

		if (key(_left))
		        GetBody(lphysics.body,cp_c_v,&vel);
			cpvperp(&nPiso,&aux);
			cpvmult(&aux, -10,&inter);
                        cpvadd(&vel, &inter,&inter);
                        inter.x=cpfclamp(inter.x, -120, 120);
                        DefBodyP(lphysics.body,CP_C_V,&inter);
                        graph++;
                        flags=0;
		end
		if (GETCOLLISIONINFO(handler,&ct))
		if (ct.points[0].normal.y>0.7)
				nPiso.x=-1.0*ct.points[0].normal.x;
				nPiso.y=-1.0*ct.points[0].normal.y;
			end
			sub=true;
			else
			sub=false;
			end
			angle=atan2(nPiso.y*1000.0,nPiso.x*1000.0)+90000;
		if (sub and key(_up) )

			GetBody(lphysics.body,cp_c_v,&vel);
			cpvmult(&nPiso, 600.0,&aux);
			cpvadd(&vel, &aux,&vel);
			vel.y=cpfclamp(vel.y, -600, 600);
			DefBodyP(lphysics.body,cp_c_v,&vel);
			cpBodyActivate(lphysics.body);
		else
		        cpv(0,-1,&nPiso);
                end
                if (graph>309)
                graph=302;
                end
                  

                UpdateVelocity();
		frame;

	end
end
