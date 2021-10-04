//Uniones rompibles, Callbacks a joints
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
	pelota;                                                //Gráfico de la pelota


Begin
        set_mode(800,600,32);
	pelota=load_png("bola40.png");

	gphysics.gravity_Y=200;                                          //Se define la gravedad en Y
	gphysics.phresolution=1;
        gphysics.bias_coef=0;

	addInanimateShape(type_line, 0.0, 600.0, 800.0,600.0, 0.0);      //Crea una línea para simular el piso y se agrega al fondo, pero no se pinta.
	addInanimateShape(type_line, 0.0, 0.0, 800.0,0.0, 0.0);
	size=addInanimateShape(type_line, 0.0, 0.0, 0.0,600.0, 0.0);
	addInanimateShape(type_line, 800.0, 0.0, 800.0,600.0, 0.0);


        write(0,400,550,4,"toma la pelota semitransparente y golpea a las demás para romper sus uniones");
        write(0,400,570,4,"También puedes tomar una de las pelotas unidas y jalar para romper las uniones");

	mouseP();
        pelota1();

        for (x=100;x<700;x+=70)
        z=0;
            for (y=100;y<300;y+=40);
                 if (y==100)
                    size=ADDSLIDEJOINT(pelota2(x,y),z,0,0,x,0,0,50);
                 else
                    size=ADDSLIDEJOINT(pelota2(x,y),z,0,0,0,0,0,50);
                 end
                 DEFCONSTRAINTF(size,CP_C_MAXFORCE,80000);
                 CONSTRAINTSETPOSTSOLVEFUNC(size,"postSolve");
                 z=son;
                 frame;
            end
        end
	while (!key(_esc))
		frame;
	end
	let_me_alone();
end

/* process borraJoint(joint)
begin
frame;
    say("removido a "+joint);
    RemoveConstraint(joint);
    say("removido b "+joint);
end */

function postSolve(int joint)
begin

// If the force is almost as big as the joint's max force, break it.
if(constraintGetImpulse(joint)/SpaceGetCurrentTimeStep() > 0.9*GETCONSTRAINTF(joint,CP_C_MAXFORCE));
	//borraJoint(joint);
	RemoveConstraint(joint);
end

end

process pelota2(x,y)
private
id3;
cpvect vel;
begin
         graph=pelota;
        //Se definen las propiedades de la pelota
	lphysics.mass=10;                  //Masa de 10
	lphysics.ELASTICITY=0;           //Elasticidad del cuerpo (0-1, preferentemente)
	lphysics.friction=0.8;             //Coeficiente de fricción del material del cuerpo
	lphysics.ShapeType=TYPE_CIRCLE;    //Forma del cuerpo, el circular es el más económico y fácil de usar.
	forceCreateBody();                 //Se forza la creación del cuerpo
        lphysics.collisionType=3;
        lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);

	loop
        	frame;
	end
end

process pelota1()
private
id3;
cpvect vel;
begin
	x=300;   y=100;   graph=pelota;
        //Se definen las propiedades de la pelota
	lphysics.mass=30;                  //Masa de 10
	lphysics.ELASTICITY=0;           //Elasticidad del cuerpo (0-1, preferentemente)
	lphysics.friction=0.8;             //Coeficiente de fricción del material del cuerpo
	lphysics.ShapeType=TYPE_CIRCLE;    //Forma del cuerpo, el circular es el más económico y fácil de usar.
	forceCreateBody();                 //Se forza la creación del cuerpo
        lphysics.collisionType=3;
	//Antes de llamar a getOptimalInertia hay que actualizar el cuerpo, ya sea forzando su creación (cuando éste aún no existe) o haciendo un frame(v) antes, con v igual a cualquier valor o sin v.
        lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);
        lphysics.sensor=false;
        flags=4;
	loop
        	frame;
	end
end

process mouseP()
private
	mouseJoint;
	x0,y0;
begin
 	lphysics.static=true;
	lphysics.ShapeType=TYPE_empty;                //No crearle cuerpos
	FORCECREATEBODY();

	graph=pelota;	size=30;	priority=5;	z=-100;
	loop
		x=mouse.x;
		y=mouse.y;
		 if (mouse.left)
			z = SpacePointQueryFirst( x, y, CP_ALL_LAYERS, CP_NO_GROUP);        //Toma el id de algún proceso que toque el punto dado por (x,y) y cuya capa esté comprendida por CP_ALL_LAYERS y pertenezca al grupo CP_NO_GROUP
			if (z and mouseJoint==0)                                            //Si z es distinto de cero y no se ha creado el joint
				WORLD2LOCAL(z, x, y,&x0, &y0);                              //Transforma las coordenadas del ratón a coordenadas locales del proceso almacenado en z
				mouseJoint=addPivotJoint2(id, z, 0,0, x0,y0);               //crea el joint entre este proceso y el proceso apuntado por z en los puntos (0,0) de este proces y el (x0, y0) del proceso que se toca. Coordenadas locales
				DefConstraintF(mouseJoint,CP_C_MAXFORCE,40000);             //Define algunos valores del joint, véase la ayuda para más información
				DefConstraintF(mouseJoint,CP_C_BIASCOEF,0.000005822);
			end
		elseif (mouseJoint<>0)                                                      //Si no se presiona el botón izquierdo del ratón eliminar el joint
			RemoveConstraint(mouseJoint);
			mouseJoint = 0;
		end
		frame;
	end
end