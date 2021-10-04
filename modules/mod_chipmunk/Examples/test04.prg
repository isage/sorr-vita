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


global
	pelota;
local
cpBB bb;       //le agrega una local para almacenar un boundingbox a cada proceso

private

	cpBB bbC;     //Boundingbox de envoltura para todos los objetos.
	id1,id2;      //ids de los objetos

Begin
        set_mode(800,600,32);
	pelota=load_png("bola40.png");

	gphysics.interval=1.0/25.0;                   //Velocidad en que el motor está actuando, normalmente se usa la inversa del valor de las imágenes por segundo

	gphysics.SLEEPTIMETHRESHOLD=infinityf();      //Time a group of bodies must remain idle in order to fall asleep
        gphysics.bias_coef=0;
	id1=cubo();                                   //Crea los dos cuerpos
	id2=cubo();

        mouseP();                                     //Crea el cuerpo del mouse

        priority=1000;
        write(0,400,50,4,"Mueve los objetos con el ratón");
	while (!key(_esc))
		delete_draw(0);
		if (cpBBIntersects(id1.bb,id2.bb))    //Si los boundingbox intersectan?
                        id1.flags=4;                  //Pon el flags de id1 a 4
                else
                        id1.flags=0;
		end

		cpBBmerge(id1.bb,id2.bb,&bb);         //Obtiene el boundingbox que envuleve a los dos procesos
		draw_rect(bb.l,bb.b,bb.r,bb.t);       //pinta el boundingbox anterior

                frame;

	end

	let_me_alone();

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
			/*	DefConstraintF(mouseJoint,CP_C_MAXFORCE,5000);             //Define algunos valores del joint, véase la ayuda para más información
				DefConstraintF(mouseJoint,CP_C_BIASCOEF,0.004);   */
			end

		elseif (mouseJoint<>0)                                                      //Si no se presiona el botón izquierdo del ratón eliminar el joint
			RemoveConstraint(mouseJoint);
			mouseJoint = 0;
		end
		frame;
	end
end

process cubo()
begin
	x=400;	y=100;	priority=father.priority+2;  graph=pelota;

	lphysics.mass=10;
	lphysics.ELASTICITY=0.1;
	lphysics.friction=0;
	lphysics.ShapeType=TYPE_CIRCLE;

	forceCreateBody();
	lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);
	loop
        ShapeCacheBB(id,&bb);                                //Actualiza el boundingbox
	draw_rect(bb.l,bb.b,bb.r,bb.t);                      //Y lo pinta
        if (x<0) x=800; end
        if (x>800) x=0; end
        if (y<0) y=600; end
        if (y>600) y=0; end
		frame;

	end
end
