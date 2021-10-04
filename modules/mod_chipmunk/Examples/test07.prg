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
import "mod_draw";
import "mod_scroll";
import "mod_chipmunk"

declare process plano(x,y,angle,static);
end

global
	pelota;
	cpVect ot;
	handler;
	fondo;
	fpg;
private
	press;
	cpSegmentQueryInfo info ;
	float distancia;
	bl;
Begin
	set_mode(800,600,32);
	fondo=load_png("fondoImg.png");
	pelota=load_png("bola40.png");
	fpg=load_fpg("graficos.fpg");
	start_scroll(0,0,fondo,0,0,0);
	
        gphysics.gravity_Y=300;
	gphysics.damping=0.9;
	gphysics.interval=1.0/25.0;
	gphysics.bias_coef=0;
	handler=COLLISIONHANDLERNEW(1,2);    //detecta donde colisionan los objetos con lphysics.collisionType 1 y 2
	
        plano(400,440,0,true);
	plano(1000,640,0,true);
	plano(1300,740,-10000,true);
	z=esfera(650,340);
	addPinJoint(z,0,0,0,900,120);

	caer(200,420);
	caer(600,500);
	caer(700,600);

	cubo(400,400);

	mouse.graph=pelota;
	mouse.size=50;

	while (!key(_esc))
		frame;
	end
	let_me_alone();
end

process plano(x,y,angle,static)
begin
	graph=new_map(300,20,32);
	z=15;
        map_clear(file,graph,rgb(0,255,0));
	ctype=c_scroll;

	lphysics.static=static;
	lphysics.collisionType=2;
	lphysics.elasticity=1;
	lphysics.friction=0.4;
	lphysics.ShapeType=TYPE_LINE;
	forceCreateBody();
	if (static)
		lphysics.mass=infinityf();
		lphysics.inertia=infinityf();
	else
		lphysics.mass=100;
		lphysics.inertia=GETOPTIMALINERTIA(TYPE_LINE,lphysics.shape);
	end
	loop
		frame;
	end
end

process esfera(x,y)
private
begin
graph=pelota;
size=400;
	ctype=c_scroll;

	lphysics.static=false;
	lphysics.collisionType=2;
	lphysics.friction=0.4;
	lphysics.ShapeType=TYPE_Circle;
	lphysics.mass=100;
	        forceCreateBody();
	lphysics.inertia=GETOPTIMALINERTIA(TYPE_circle,lphysics.shape);
	loop
		frame;
	end
end

process caer(x,y)
private
begin
	graph=new_map(100,20,32);
	z=15;
        map_clear(file,graph,rgb(250,0,0));
	ctype=c_scroll;

        lphysics.static=false;
	lphysics.collisionType=2;
	lphysics.elasticity=0;
	lphysics.friction=0;
	lphysics.ShapeType=TYPE_box;
	lphysics.mass=1;
        forceCreateBody();
        lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);
     //   signal(id,s_freeze);
 	sleep(id);
	loop
	if (key(_y))
		sleep(id);
		end

	//	say(issleeping(id));
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
	dat[]=0,-20,20;//se puede crear un arreglo con los parámetros para la creación del cuerpo físico y asignarse la dirección del arreglo a la variable local lphysics.params para crear el cuerpo de acuerdo a éstos. Los parámetros son los mismos que los que se usan en las funciones que hacen este trabajo y en el mismo orden (addCircleShape, por ejemplo).
begin
        file=fpg;
	graph=302;
	priority=father.priority-2;
	ctype=c_scroll;
	scroll.camera=id;

	lphysics.static=false;
	lphysics.mass=10;
	lphysics.inertia= cpMomentForCircle(lphysics.mass, 0, 20, cpv(0,-20,&aux));
	lphysics.params=&dat;                     //parámetros para la creación del lphysics.shape: centro 0,-20 y 20 de radio.
	lphysics.elasticity=0;
	lphysics.friction=0;
	lphysics.collisionType=1;
	lphysics.ShapeType=TYPE_circle;
	FORCECREATEBODY(); //forza la creación del cuerpo físico inmediatamente para comenzar a usarlo

	loop

		if (key(_right))
			GetBody(lphysics.body,cp_c_v,&vel);      //Obtiene la velocidad del cuerpo
			cpvperp(&nPiso,&aux);                    //guarda en aux el vector perpendicular a nPiso (Normal al piso)
			cpvmult(&aux, 10,&inter);                //Multiplica aux por 10 y lo guarda en inter
			cpvadd(&vel, &inter,&inter);             //Suma a vel inter y lo guarda en inter
			inter.x=cpfclamp(inter.x, -120, 120);    //Restringe inter.x entre -120 a 120
			DEFBODYP(lphysics.body,CP_C_V,&inter);   //Define la velocidad del cuerpo igual a inter
			graph++;
			flags=1;

		end
		if (key(_left))
			GetBody(lphysics.body,cp_c_v,&vel);
			cpvperp(&nPiso,&aux);
			cpvmult(&aux, -10,&inter);               //Igual al anterior, pero multiplica aux por -10 para mover el proceso en la otra dirección
			cpvadd(&vel, &inter,&inter);
			inter.x=cpfclamp(inter.x, -120, 120);
			DEFBODYP(lphysics.body,CP_C_V,&inter);
			graph++;
			flags=0;
		end
		if (GETCOLLISIONINFO(handler,&ct))               //Si se toca el piso
			if (ct.points[0].normal.y>0.7)           //Se restringe qué inclinación del piso se considera para obtener las normales (Este vector es unitario)
				nPiso.x=-1.0*ct.points[0].normal.x;   //Se guarda la normal
				nPiso.y=-1.0*ct.points[0].normal.y;
			end
			sub=true;
		else
			sub=false;
		end

		angle=atan2(nPiso.y*1000.0,nPiso.x*1000.0)+90000;     //Le asigna un ángulo al personaje de acuerdo a al inclinación del piso

		if (sub and key(_up) )                                //si quieres saltar y estás tocando el piso
			GetBody(lphysics.body,cp_c_v,&vel);           //Se guarda la velocidad en vel
			cpvmult(&nPiso, 600.0,&aux);                  //Se multiplica la normal al piso por 600 (altura del salto)
			cpvadd(&vel, &aux,&vel);                      //Se suma el resultado anterior a la velocidad actual
			vel.y=cpfclamp(vel.y, -600, 600);             //Se limita la velocidad del salto
			DEFBODYP(lphysics.body,cp_c_v,&vel);          //y se hace saltar asignándole la velocidad almacenda en vel al cuerpos
		//	cpBodyActivate(lphysics.body);                //Cuando se hagan cambios a las propiedades de un cuerpos, es necesario llamar esta función
		else
			cpv(0,-1,&nPiso);
		end
		if (graph>309)
			graph=302;
		end
		UpdateVelocity();                                     //Actualiza la velocidad del proceso para que disminuya con el paso del tiempo. sólo es necesario usar esta función cuando se modifican los valores del cuerpo directamente mediante funciones como DEFBODY* o setMass
		frame;

	end
end
