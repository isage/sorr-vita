import "mod_math"
	import "mod_draw"
	import "mod_map"
	import "mod_screen"
	import "mod_chipmunk"
	import "mod_video"
	import "mod_text"
	import "mod_proc"
	import "mod_key"
	import "mod_wm"
	import "mod_say"
	import "mod_regex"
	import "mod_string"
	import "mod_map"
	import "mod_grproc"
	import "mod_rand"
	import "mod_file"
	import "mod_sound"
	import "mod_timers"
	import "mod_time"
	import "mod_dir"
	import "mod_scroll"

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
	fondo=load_png("fondoHielo.png");
	fpg=load_fpg("graficos.fpg");
	start_scroll(0,0,fondo,0,0,0);       //Se inicia el scroll
                                    say("ui");
        gphysics.gravity_Y=300;              //Define la gravedad a 300
	gphysics.damping=0.9;                //Disminuye la viscosidad a 0.9
	handler=COLLISIONHANDLERNEW(1,0);    //detecta donde colisionan los objetos con lphysics.collisionType 1 y 0. De tal forma que nos permitirá saber cuando nuestro protagonista toque el suelo
        gphysics.bias_coef=0;
	generaTerreno("piso.cpt");           //Se genera el terreno
        prota(100,100);                      //Se genera el protagonista
        
        write(0,400,30,4,"Presiona espacio para ver el terreno físico");
	while (!key(_esc))
	        if (key(_space))             //si presionas espacio
	        pintaTerreno("piso.cpt");    //Pinta el terreno en el que se colisiona
	        end
		frame;
	end
	let_me_alone();                      //Si se presiona escape, se finaliza todo
end

function generaTerreno(string ruta);         //Recibe una string con la ruta donde está el cpt
private
	float x0=-1,x1=-1,y0=-1,y1=-1;
	string cad;
	string cadenas[2];
	otro_os;
	cant;
	cont8;
begin
	otro_os=fopen(ruta,O_READ);          //abre el cpt
	if (not otro_os)                     //Si no pudo abrirlo retorna false
		return false;
	end
	x0=y0=x1=y1=-1;                      //inicializa las variables de la posición a -1
	cad=fgets(otro_os);                  //Lee encabezado
	cad=fgets(otro_os);                  //lee la cantidad de puntos de control
	cant=atoi(cad);                      //Almacena la cantidad de puntos de control en la variable cant
	cant--;
	cad=fgets(otro_os);                  //leer punto 0  e ignorarlo
	for (cont8=0;cont8<cant;cont8++)     //Para todos los puntos que faltan
		x1=x0;                       //guarda en x1 el valor de la última coordenada leída de x
		y1=y0;                       //y en y1 la última coordenada leída en y
		cad=fgets(otro_os);          //lee el siguiente punto
		if (cad=="-1 -1")            //si tiene -1 -1, reiniciar la línea
			x0=y0=x1=y1=-1;      //La línea se reinicia colocando las cuatro variables a -1
		else                         //De lo contrario
			split(" ",cad,&cadenas,2);   //Dividir la cadena en 2 usando el espacio
			x0=atoi(cadenas[0]);         //recoger el valor de x
			y0=atoi(cadenas[1]);         //recoger el valor de y
		end
		if (x1<>-1)                          //si la x1 está definida, entonces ya tenemos una línea
			z= addInanimateShape(type_line,x1,y1,x0,y0,0.0);       //se agrega al terreno la línea
		end
	end
	fclose (otro_os);                                                      //si ya se terminó de leer, se cierra el archivo
	return true;                                                           //y se retorna true
end


//Esta función pinta líneas con draw para ver si nuestro terreno quedó bien, 
//su funcionamiento es el mismo que el de la función anterior, así que no lo comentaré

function pintaTerreno(string ruta);
private
	float x0=-1,x1=-1,y0=-1,y1=-1;
	string cad;
	string cadenas[2];
	otro_os;
	cant;
	cont8;
begin
	delete_draw(0);
	drawing_color(rgb(255,0,0));
	otro_os=fopen(ruta,O_READ);
	if (not otro_os) 
		return false;
	end
	x0=y0=x1=y1=-1;
	cad=fgets(otro_os);
	cad=fgets(otro_os);
	cant=atoi(cad);
	cant--;
	cad=fgets(otro_os); //leer punto 0
	for (cont8=0;cont8<cant;cont8++);
		x1=x0;
		y1=y0;
		cad=fgets(otro_os);
		if (cad=="-1 -1")
			x0=y0=x1=y1=-1;
		else
			split(" ",cad,&cadenas,2);
			x0=atoi(cadenas[0]);
			y0=atoi(cadenas[1]);
		end
		if (x1<>-1)
			draw_line(x1-scroll.x0,y1-scroll.y0,x0-scroll.x0,y0-scroll.y0);
		end
	end
	fclose (otro_os);
	return true;
end

//Protagonista
process prota(x,y)
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
	ctype=c_scroll;
	scroll.camera=id;

	lphysics.static=false;
	lphysics.mass=10;
	lphysics.inertia= cpMomentForCircle(lphysics.mass, 0, 20, cpv(0,-20,&aux)); //define el momento, este valor es muy importante
	lphysics.params=&dat;                     //parámetros para la creación del lphysics.shape: centro 0,-20 y 20 de radio.
	lphysics.elasticity=0;
	lphysics.friction=0;
	lphysics.collisionType=1;
	lphysics.ShapeType=TYPE_circle;
	FORCECREATEBODY(); //forza la creación del cuerpo físico inmediatamente para comenzar a usarlo
        cpv(0,-1,nPiso);   //el personaje comienza de pie
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

			if (ct.points[0].normal.y>0.5)           //Se restringe qué inclinación del piso se considera para obtener las normales (Este vector es unitario)
				nPiso.x=-1.0*ct.points[0].normal.x;   //Se guarda la normal
				nPiso.y=-1.0*ct.points[0].normal.y;
			end
			sub=true;
		else
			sub=false;
		end

		angle=atan2(nPiso.y*1000.0,nPiso.x*1000.0)+90000;     //Le asigna un ángulo al personaje de acuerdo a al inclinación del piso
		if (flags==0)
			angle*=-1;                                    //Corrige los ángulos
		end
		if (sub and key(_up) )                                //si quieres saltar y estás tocando el piso
			GetBody(lphysics.body,cp_c_v,&vel);           //Se guarda la velocidad en vel
			cpvmult(&nPiso, 600.0,&aux);                  //Se multiplica la normal al piso por 600 (altura del salto)
                        cpvadd(&vel, &aux,&vel);                      //Se suma el resultado anterior a la velocidad actual
                        vel.y=cpfclamp(vel.y, -600, 600);             //Se limita la velocidad del salto
			DEFBODYP(lphysics.body,cp_c_v,&vel);          //y se hace saltar asignándole la velocidad almacenda en vel al cuerpos
			cpBodyActivate(lphysics.body);                //Cuando se hagan cambios a las propiedades de un cuerpos, es necesario llamar esta función
		end
		if (graph>309)
			graph=302;
		end
		if (y>600)
		y=100; x=100;
		end
		UpdateVelocity();                                     //Actualiza la velocidad del proceso para que disminuya con el paso del tiempo. sólo es necesario usar esta función cuando se modifican los valores del cuerpo directamente mediante funciones como DEFBODY* o setMass
		frame;

	end
end
