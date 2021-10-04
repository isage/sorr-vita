//agua -No terminado-
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
import "mod_timers";

declare function float fagua(x,t,float k,w,a,t2);
end

global
	pelota;                                                //Gráfico de la pelota
	float CA;
	int contador;
        gotas;
	agua;
Begin
        set_mode(800,600,32);
	pelota=load_png("bola40.png");
	gotas=new_map(40,40,32);
	map_clear(0,gotas,rgba(0,0,255,130));

	gphysics.gravity_Y=200;                                          //Se define la gravedad en Y
	gphysics.phresolution=1;

	addInanimateShape(type_line, 0.0, 600.0, 800.0,600.0, 0.0);      //Crea una línea para simular el piso y se agrega al fondo, pero no se pinta.
	addInanimateShape(type_line, 0.0, 0.0, 800.0,0.0, 0.0);
	addInanimateShape(type_line, 0.0, 0.0, 0.0,600.0, 0.0);
	addInanimateShape(type_line, 800.0, 0.0, 800.0,600.0, 0.0);

	addInanimateShape(type_line, 100.0, 350.0, 100.0,450.0, 0.0);
	addInanimateShape(type_line, 700.0, 350.0, 700.0,450.0, 0.0);
	addInanimateShape(type_line, 100.0, 450.0, 700.0,450.0, 0.0);
        write_var(0,100,100,4,CA);
	mouseP();
        SPACEADDCOLLISIONHANDLER(3,1,"beginN","presolve","","",null,null,null,null); //un sensor, como el agua, no llama a postsolve. Si begin o presolve retornan false tampoco se llama a postsovle
        agua();
        pelota1();
	while (!key(_esc))
                /* clear_screen();
	        from z=10 to 790;
                    put_pixel(z,300+fagua(z-400,timer[0]/5,10000.0,20000.0,80.0,2.0),rgb(255,0,0));
	        end */
		frame;
		contador+=1000;
	end
	let_me_alone();
end

function beginN(int arb, int * p )
private
float fx,fy;
begin
         GETARBITERPOINT(arb,0,&fx,&fy);
         charpea((int)fx,(int)fy,155000,3,100);
         charpea((int)fx,(int)fy,25000,3,100);
	z=MODARBITERPROCESSA(ARB);
	ResetForces(z);
	ApplyForce(z, 0, gphysics.gravity_Y*-10,0,0);
        DefBodyP(z.lphysics.body,cp_c_v,&cpvzero);

return true;
end


process charpea(x,y,angle,av,int size)
begin
frame;  //No se pueden crear objetos físicos desde un proceso o función llamado desde un callback si no hasta que ha pasado un frame...
graph=gotas;
lphysics.sensor=true;
  lphysics.collisionType=6;
  lphysics.ShapeType=TYPE_circle;
  lphysics.mass=10;
  FORCECREATEBODY();
  lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);
  for (;size>20;size--)
     advance(av);
     if ((size % 10)==0)
        charpea(x,y,angle+rand(-10000,10000),av+rand(-1,1),size+rand (-3,-15));
     end
     frame;
  end
end


function presolve(int arb, *p)
private
cpvect cpv1;
begin
z=MODARBITERPROCESSA(ARB);
     ResetForces(z);
     cpv1.x=0;
     cpv1.y=350-z.y;
     CA=cpv1.y=cpfclamp(cpv1.y,-20.0,20.0)-(sin(contador)*10+10);
     DefBodyP(z.lphysics.body,cp_c_v,&cpv1);
end

process agua()
begin
  lphysics.sensor=true;
  lphysics.collisionType=1;
  lphysics.static=true;

  graph=new_map(600,100,32);
  map_clear(file,graph,0880000FFh);
  x=400;
  y=400;
  lphysics.ShapeType=TYPE_box;

loop
  frame;
end
  onexit;
  unload_map(file,graph);
end

function float fagua(x,t,float k,w,a,t2)
begin
//say((a/(float)x)*cos(k*x+w*t));
if (x<0)
return -abs(a/abs((float)x/(float)t2)*sin(k*x+w*t));
else
return -abs(a/abs((float)x/(float)t2)*sin(k*x+w*-t));
end
end

process pelota1()
private
id3;
cpvect vel;
begin
	x=300;   y=100;   graph=pelota;
        //Se definen las propiedades de la pelota
	lphysics.mass=1;                  //Masa de 10
	lphysics.ELASTICITY=0.9;           //Elasticidad del cuerpo (0-1, preferentemente)
	lphysics.friction=0.8;             //Coeficiente de fricción del material del cuerpo
	lphysics.ShapeType=TYPE_CIRCLE;    //Forma del cuerpo, el circular es el más económico y fácil de usar.
	forceCreateBody();                 //Se forza la creación del cuerpo
        lphysics.collisionType=3;
	//Antes de llamar a getOptimalInertia hay que actualizar el cuerpo, ya sea forzando su creación (cuando éste aún no existe) o haciendo un frame(v) antes, con v igual a cualquier valor o sin v.
        lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);
        lphysics.sensor=false;
//        areaCirculo=CPAREAFORCIRCLE(20,0);
	loop
	GetBody(lphysics.body,cp_c_f,&vel);
            //  ResetForces(id);
           updateVelocity();
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
				DefConstraintF(mouseJoint,CP_C_MAXFORCE,50000);             //Define algunos valores del joint, véase la ayuda para más información
				DefConstraintF(mouseJoint,CP_C_BIASCOEF,0.000005822);
			end
		elseif (mouseJoint<>0)                                                      //Si no se presiona el botón izquierdo del ratón eliminar el joint
			RemoveConstraint(mouseJoint);
			mouseJoint = 0;
		end
		frame;
	end
end