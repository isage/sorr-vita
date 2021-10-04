//Generación de terrenos automáticamente a partir de un mapa...
//No funciona ahora
import "mod_key";
import "mod_text";
//No usar linea de 2 o 3 pixels de ancho, de lo contrario el algoritmo fallará
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

        int num = 4;               //Número de vértices del carro
	cpVect verts[] =           //Lista con vértices del cuerpo del carro
	-40,-15,
	-40, 15,
	40, 15,
	40,-15;
	terreno;
	puntos;
	pelota;
    mundo;
    umbral=5;

function pLinea(cpVect * c1,cpVect *c2)
begin
  draw_line(c1.x,c1.y,c2.x,c2.y);
  z=addInanimateShape(type_line, c1.x,c1.y,c2.x,c2.y, 1);
  DefShapeI(z,CP_C_LAYERS,NOT_GRABABLE_MASK);
     
end

function genTerr(int l)
private
    cantL, cantP,objeto,i;
    cpVect * c1;
    cpVect * c2;
    int ln;
begin
       cantL=cantElement(l);
       for (z=0;z<cantL;z++)
           objeto=getElement(l,z);
           cantP=cantElement(objeto);
             for (i=0;i<cantP-1;i++)
                  c1=getElement(objeto,i);
                  c2=getElement(objeto,i+1);
                 pLinea(c1,c2);
                 ln++;
                 draw_fcircle(c1.x,c1.y,4);
             end
             draw_fcircle(c2.x,c2.y,4);
       end
       say("lineas generadas = "+ ++ln);
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
	loop
	    frame;
	end
	onexit;
	unload_map(file,graph);
end

function carro(x,y)
private
body1,body2;
begin
	body1 = addWheel(50+x,100+y);
	body2 = addWheel(110+x,100+y);
	z=      addChassis(80+x,60+y);
	addGrooveJoint(z,body1,-30,10,-30,40,0,0);
	addGrooveJoint(z,body2,30,10,30,40,0,0);

	addDampedSpring(z, body1,-30,0,0,0,50,20,1.5);
	addDampedSpring(z, body2,30,0,0,0,50,20,1.5);
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

Begin
  set_mode(800,600,32);
  gphysics.gravity_y=100;          //gravedad en y
  gphysics.interval=1.0/25.0;
  gphysics.bias_coef=0.3;
  write(0,100,100,0,"Selecciona el umbral para aproximar el mapa presionando un número del 1 al 9");
  ascii=0;
  while (ascii<49 or ascii>57)
    frame;
  end
  if(ascii<=57 and ascii>=49)
    umbral=ascii-48;
  end
  delete_Text(0);

  terreno=load_png("buss.png");
  x=400; y=300;
  graph=terreno;
  mundo=generateAproxPoly(0,terreno,umbral);
  genTerr(mundo);
  deleteAproxPoly(mundo);
  pelota=load_png("pelota.png");
  carro(400,200);
  mouseP();
  write_Var(0,100,100,4,mouse.x);
  write_Var(0,100,120,4,mouse.y);
  write(0,100,10,0,"Umbral seleccionado = "+umbral);
  loop
    if (key(_esc))
      break;
    end
    frame;
  end
  unload_map(file,graph);
  unload_map(file,pelota);
  let_me_alone();
end