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
//36%

//tamaño de las partículas
#define tam 15

//cantidad de partículas
#define tamsidcT 60

//ancho de pantalla
#define ancho 800

//alto de pantalla
#define alto 600

declare function float fagua(x,y)
end
declare function float mag(float x1,y1)
end
declare function pintaAgua(float umb)
end
declare function float cuad(float a)
end


global
	pelota;                                                //Gráfico de la pelota
	ids[tamsidcT];
	color;
	WaterS w;
	terreno;
	puntos;
	despx,despy;
	estructuraId;
	tamsidc=0;


function pLinea(cpVect * c1,cpVect * c2)
begin
  //c1.x=c1.x+despx; c1.y=c1.y+despy;
  //c2.x=c2.x+despx; c2.y=c2.y+despy;
  draw_line(c1.x+despx,c1.y+despy,c2.x+despx,c2.y+despy);
  z=addInanimateShape(type_line, c1.x+despx,c1.y+despy,c2.x+despx,c2.y+despy, 1);
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
             end
       end
       say("lineas generadas = "+ ++ln);
end





Begin
        set_mode(ancho,alto,32);
        set_fps(30,0);
	pelota=new_map(tam,tam,8);
	map_clear(0,pelota,5);
        graph=new_map(320,240,32);
        terreno=load_png("terreno320x240.png");
        put_screen(0,terreno);
        puntos=generateAproxPoly(0,terreno,4);
        
  
        despx=(ancho-graphic_info(0,terreno,g_width))/2;//estructuraId.x
        despy=(alto-graphic_info(0,terreno,g_height))/2;//estructuraId.y
        genTerr(puntos);
        deleteAproxPoly(puntos);
        write_Var(0,400,300,4,w.size);//write_Var(0,400,330,4,mouse.y);
	gphysics.gravity_Y=200;                                          //Se define la gravedad en Y

	addInanimateShape(type_line, 0.0, alto, ancho,alto, 1.0);      //Crea una línea para simular el piso y se agrega al fondo, pero no se pinta.
	addInanimateShape(type_line, 0.0, 0.0, ancho,0.0, 1.0);
	addInanimateShape(type_line, 0.0, 0.0, 0.0,alto, 1.0);
	addInanimateShape(type_line, ancho, 0.0, ancho,alto, 1.0);
	z=0;

        mouseP();
        color=0ff0909ffh;//find_color(60,60,200);
        x=ancho/2;
        y=alto/2;
        despx=x-(320/2);
        despy=y-(240/2);
        z=-10;
        alpha=100;
       // put_screen(0,load_png("fondo.png"));
        //say(sizeof(w));
        w.ids=&ids;
        w.kLinearViscocity=0.5;
        w.kQuadraticViscocity=0.25;
        w.kSurfaceTension=0.0004;
        w.kNorm=2.2104;
        w.kNearNorm=3.3157;
        w.kRestDensity=82;
        w.kStiffness=0.08;
        w.kNearStiffness=0.1;
        w.KH=120;
        
        cuadro(450,231);
        tamsidc=0;
        while (!key(_esc))
        if (rand(0,3)==2 and tamsidc<tamsidcT)
          ids[tamsidc]=gota(519,211);
          tamsidc++;
          w.size=tamsidc;
        end

        //pintaAgua(0.2);
        map_clear(file,graph,0);
        draw_water(file,graph,color,&ids,tamsidc,tam,0.3,15,despx,despy);
        EMULATE_WATER(&w);
              frame;
	end
	let_me_alone();
end


process cuadro(x,y)
begin
	graph=new_map(20,40,32);
        lphysics.mass=1;       //Todo objeto estático debe tener su masa e inercia igual a infinito
	lphysics.elasticity=0.2;           //La elasticidad va, preferentemente, de 0 a 1, y se refiere a qué tanto el objeto rebotará al contacto con otros
	lphysics.friction=1.0;             //La fricción va, preferentemente, de 0 a 1, y se refiere a cuánta velocidad perderá el objeto al friccionar. Una friccón de cero hará que el objeto no gire al tocar los otros objetos
	lphysics.group=1;                //Grupo al que pertenece el proceso. Procesos del mismo grupo no colisionan
	lphysics.ShapeType=TYPE_box;  //Tipo de cuerpo, esto inicializa la creación del cuerpo
	forceCreateBody();
        lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);
	map_clear(file,graph,rgb(255,100,30));
	z=-15;
	loop
		frame;
	end
end

process gota(x,y)
begin
  graph=pelota;
  alpha=0;
  lphysics.mass=0.0005;                  //Masa de 10
  lphysics.ELASTICITY=0.0;           //Elasticidad del cuerpo (0-1, preferentemente)
  lphysics.friction=0.0;             //Coeficiente de fricción del material del cuerpo
  lphysics.ShapeType=TYPE_CIRCLE;    //Forma del cuerpo, el circular es el más económico y fácil de usar.
  forceCreateBody();                 //Se forza la creación del cuerpo
  lphysics.inertia=GETOPTIMALINERTIA(lphysics.ShapeType,lphysics.shape);
//  say("shape "+lphysics.shape);
  set_efector(lphysics.shape,1);
  loop
  if (x>(ancho+320)/2)
     x=519;
     DEFBODYP(lphysics.body,CP_C_V,&cpvzero);
  end
  if (y>(alto+240)/2)
     y=211;
     DEFBODYP(lphysics.body,CP_C_V,&cpvzero);
  end

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

	graph=pelota;	size=90;	priority=5;	z=-100;
	loop
		x=mouse.x;
		y=mouse.y;
		 if (mouse.left)
			z = SpacePointQueryFirst( x, y, CP_ALL_LAYERS, CP_NO_GROUP);        //Toma el id de algún proceso que toque el punto dado por (x,y) y cuya capa esté comprendida por CP_ALL_LAYERS y pertenezca al grupo CP_NO_GROUP
			 //say(z);
			if (z and mouseJoint==0)
                                                                //Si z es distinto de cero y no se ha creado el joint
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