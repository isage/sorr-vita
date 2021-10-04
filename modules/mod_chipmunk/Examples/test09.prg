//PRG, ejemplo para el motor de física. Máquina TheoJansen
//Originalmente escrita por Scott Lembcke y adaptada a Bennu por prg

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

#define ofss 30
#define side 30
#define seg_radius 3
#define crank_radius 13
#define num_legs  2
#define leg_mass 1

global
	cpVect arrowDirection,ot;
	float coef,rate;
	chassis,crank,motor;

function float rad2deg(float ang)
begin
	return (ang*180000)/CM_PI;
end

process leg(x,y,x0,y0)
private
	datos[]=0, 0,0,-1*side, seg_radius;
begin
	lphysics.params=&datos;
	lphysics.inertia=cpMomentForSegment(leg_mass, 0.0,0.0,0.0,-1.0*side);
	lphysics.ShapeType=TYPE_LINE;
	FORCECREATEBODY(); //forza la creación del cuerpo físico inmediatamente para comenzar a usarlo

	ADDPIVOTJOINT2(chassis,id,x0,y0,0,0);
	graph=new_map(seg_radius,side,32);
	map_clear(file,graph,rgb(100,45,25));
	set_center(file,graph,seg_radius/2,side-1);
	loop
		frame;
	end
end

process lowerLeg(x,y)
private
	datos[]=0, 0,0,side, seg_radius;
	circ;
begin
	lphysics.params=&datos;
	lphysics.inertia=cpMomentForSegment(leg_mass, 0.0,0.0, 0.0, side);
	lphysics.ShapeType=TYPE_LINE;
	FORCECREATEBODY(); //forza la creación del cuerpo físico inmediatamente para comenzar a usarlo
	lphysics.group=1;
	circ=ADDCIRCLESHAPE(0.0, side,seg_radius*2.0);
	defShapef(circ,cp_c_e,0.0);
	defShapef(circ,cp_c_u,1);
	graph=new_map(seg_radius,side,32);
	map_clear(file,graph,rgb(100,45,25));
	set_center(file,graph,seg_radius/2,0);

	loop
		frame;
	end
end


process chassis(x,y)
private
	datos[]= (ofss*-1), 0,ofss, 0,seg_radius;
	cpVect a,b;
begin
	lphysics.params=&datos;
	lphysics.mass = 2.0;
	lphysics.inertia=cpMomentForSegment(lphysics.mass, -1.0*ofss, 0.0, (float)ofss, 0.0);
	lphysics.ShapeType=TYPE_LINE;
	lphysics.group = 1;
	graph=new_map(ofss*2,seg_radius,32);
	map_clear(file,graph,rgb(100,100,25));
	FORCECREATEBODY(); //forza la creación del cuerpo físico inmediatamente para comenzar a usarlo
	loop
		frame;
	end
end



process crank(x,y)
private
	datos[]=0,0,crank_radius;
begin
	lphysics.params=&datos;
	lphysics.mass = 1.0;
	lphysics.inertia=CPMOMENTFORCIRCLE(lphysics.mass, crank_radius, 0.0,0.0,0.0);
	lphysics.ShapeType=TYPE_Circle;
	lphysics.group = 1;
	graph=new_map(crank_radius,crank_radius,32);
	map_clear(file,graph,rgb(100,45,25));
	FORCECREATEBODY(); //forza la creación del cuerpo físico inmediatamente para comenzar a usarlo
	loop
		frame;
	end
end

function make_leg(float oft, cpVect anchor)
private
	low,up,constraint;
	float diag;
begin
	// make leg
	up=leg(chassis.x+oft, chassis.y,oft, 0.0);

	// lower leg
	low=lowerLeg(chassis.x+oft, chassis.y+side);
	ADDPINJOINT(chassis, low, oft, 0,0,0);

	ADDGEARJOINT(up, low, 0.0, rad2deg(1.0));

	diag = sqrt(side*side + oft*oft);

	constraint =ADDPINJOINT(crank, up, anchor.x,anchor.y, 0.0, -1*side);
	SETPINJOINTPROPERTIES(constraint,CP_C_DIST, diag);                         //Modifica las propiedades de un constraint tipo pin
	constraint =ADDPINJOINT(crank, low, anchor.x,anchor.y, 0,0);
	SETPINJOINTPROPERTIES(constraint,CP_C_DIST, diag);

end



private
	float distancia;
	bl;

	cpVect a,b;
Begin
	set_mode(800,600,32);
	gphysics.gravity_Y=500;
	gphysics.interval=1.0/25.0;
	gphysics.iterations=20;

	draw_line(-5,0,0,600);
	lphysics.shape = addInanimateShape(type_line,-5.0,0.0,0.0,600.0,0.0);  //Agrega los límites de la pantalla.
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);
	DefShapeF(lphysics.shape,CP_C_E,0);
	DefShapeF(lphysics.shape,CP_C_U,1.0);

	draw_line(799,0,799,600);
	lphysics.shape = addInanimateShape(type_line, 799.0,0.0,799.0,600.0, 0.0);
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);
	DefShapeF(lphysics.shape,CP_C_E,0);
	DefShapeF(lphysics.shape,CP_C_U,1.0);

	draw_line(0,599,800,599);
	lphysics.shape = addInanimateShape(type_line, 0.0,600.0,800.0,600.0, 0.0);
	DefShapeI(lphysics.shape,CP_C_LAYERS,NOT_GRABABLE_MASK);
	DefShapeF(lphysics.shape,CP_C_E,0);
	DefShapeF(lphysics.shape,CP_C_U,1.0);


	chassis=chassis(400,500);
	crank=crank(400,500);
	ADDPIVOTJOINT2(chassis,crank,0,0,0,0);

        write(0,300,10,4,"Usa las flechas del teclado para mover el cuadrúpedo. Con arriba y abajo aceleras.");

	for( z=0; z<num_legs; z++)
		cpvforangle((float)(2*z)/(float)num_legs*CM_PI,&a);
		cpvmult(&a, crank_radius,&a);
		cpvforangle((float)(2*z+1)/(float)num_legs*CM_PI,&b);
		cpvmult(&b, crank_radius,&b);
		make_leg( ofss,  &a);
		make_leg( -1*ofss,  &b);
	end
	motor=ADDSIMPLEMOTOR(chassis,crank,rad2deg(6.0));

        while (!key(_esc))
        arrowDirection.y=0;
        arrowDirection.x=0;
		if (key(_up))
			arrowDirection.y=1;
		end

		if (key(_down))
			arrowDirection.y=-1;
		end

		if (key(_left))
			arrowDirection.x=1;
		end

		if (key(_right))
			arrowDirection.x=-1;
		end

		coef = (2.0 + arrowDirection.y)/3.0;
		rate = rad2deg(arrowDirection.x*10.0*coef);
		SETSIMPLEMOTORPROPERTIES(motor,CP_C_RATIO,rate);                //Modifica las propiedades de un motor
		DEFCONSTRAINTF(motor,CP_C_MAXFORCE,(rate) ? 100000.0 : 0.0);    //Define una propiedad de un constraint con valores flotantes

		frame;
	end
	let_me_alone();
end





