
// what will be printed?
whatprint = 0; // [0:cover, 1: pokrywka, 2: speaker holder, 3: pcb holder]

module symetric(n=0)
{
    for (symetri3s = [0,1]) mirror([
        (n==0) ? symetri3s : 0,
        (n==1) ? symetri3s : 0,
        (n==2) ? symetri3s : 0]) children();
}
module icube(x,y,z)
{
    translate([-x/2, -y/2, 0]) cube([x,y,z]);
}

$fn=32;


// speaker dimensions
mi_d=15.8;
mi_l=24.8;
mi_h=4;
mi_left= mi_d/2+2;



wall = 1.6;
mainlen_ex = 75;
mainlen_in = mainlen_ex-2*wall;
mainwid_in = 30;
mainwid_ex=mainwid_in + 2*wall;
mainh_in = 17.4;
botom=2.1;
gtop=1.5;

// pcb holes
hdx=8*2.54;
hdy=10*2.54;
hlx=18.5+2*2.54;

pip1=[[12,24]];
pip2=[[12,12],[35,7]];

xkey=59;
wkey=6.4;
wswitch = 9.6;
piph=9;

module topik()
{
    translate([-wall,0,mainh_in+1]) difference()
    {
        union() {
            translate([0,-mainwid_ex/2,0]) cube([mainlen_ex,mainwid_ex,gtop]);
            translate([mainlen_in+wall,-wswitch/2+0.6,-2.4]) cube([wall,wswitch-1.2,2.5]);
            translate([pip2[1][0]+pip2[1][1]+0.4,mainwid_in/2,-6]) cube([7.2,wall,6.1]);
        }
        for (p=pip2) translate([p[0]-0.6,mainwid_in/2-1,-1]) cube([p[1]+1.2,5,5]);
        mirror([0,1,0]) translate([pip1[0][0]-0.6,mainwid_in/2-1,-1]) cube([pip1[0][1]+1.2,6,6]);
        for (x = [3, mainlen_ex-3]) symetric(1) {
            translate([x,mainwid_ex/2-3,-1]) {
                cylinder(d=2.4,h=10);
                translate([0,0,gtop-0.2]) cylinder(d1=2.5, d2=6.5, h=2);
            }
        }
    }
}
module spikiper()
{
    difference() {
        translate([-mi_left,-13.5,0]) cube([mi_left*2-2,27,1.3]);
        symetric(1) translate([mi_left-2-5,13.5-4,-1]) cube(6);
    symetric(1) translate([-mi_left+3,mainwid_in/2-3,-1]) cylinder(d=2.4,10);
        translate([0,0,-5]) hull() symetric(1) translate([0,(mi_l-mi_d)/2,0]) cylinder(d=mi_d-6,h=10);
    }
    icube(mi_left*2-5,6,0.9);
    cylinder(d=6,h=0.9+1.2);

}

module buda()
{
    module boks(h)
    {
        difference() {
            translate([-wall,-mainwid_ex/2,-botom]) cube([mainlen_ex,mainwid_ex,botom+h]);
            translate([0,-mainwid_in/2,0]) 
                cube([mainlen_in,mainwid_in, h+1]);
        }
    }
    
    module hole(ha)
    {
        hb=ha+2.1-0.9;
        cylinder(d=1.8,h=hb+1);
        cylinder(d=2.4,h=2);
        translate([0,0,hb-0.4]) cylinder(d1=1.8,d2=2.4,h=0.6);
    }
    
    difference() {
        union() {
            boks(mainh_in);
            // głośnik
            translate([mainlen_in-2*mi_left,-mainwid_in/2,-2.1+0.9]) cube([20.1,mainwid_in,1+mi_h]); 
            
            // dystans pod płytkę
            for (x=[hlx,hlx+hdx]) symetric(1) translate([x,hdy/2,-0.1]) {
                cylinder(d=4.5, h=2.1);
                translate([-4.5/2,0,0]) cube([4.5,3.5,2.1]);
            }
            // skrzydełka
            for (p=pip2) translate([p[0]-wall,mainwid_in/2,0]) cube([p[1],wall,mainh_in+piph]);
            for (p=pip1) translate([p[0]-wall,-mainwid_ex/2,0]) cube([p[1],wall,mainh_in+piph]);
             // mocowania topu prawe
            symetric(1) translate([mainlen_in+wall-6,mainwid_ex/2-6,mainh_in-5]) hull() {
                cube([5.5,5.5,5]);
                translate([5,5,-5]) cube(0.5);
            }
            // mocowania topu lewe
            symetric(1) translate([-wall,mainwid_ex/2-6,mainh_in-5]) hull() {
                cube([6,6,5]);
                translate([0,5,-5]) cube(0.5);
            }
            
            // klawisz
            translate([xkey-wkey/2-4.1,mainwid_in/2-3.6,mainh_in-wkey]) hull() {
                cube([wkey+8.2,4,wkey-1]);
                translate([4,4,-4]) cube([wkey+2,0.1,0.1]);
            }
            // wyłącznik
            *translate([-wall,-wswitch/2-4.1,mainh_in-6.8]) cube([wall+2+1.6,wswitch+8.4,6]);
            translate([mainlen_in-3.5,-wswitch/2-4.2,mainh_in-6.8]) {
                hull() {
                    cube([4,wswitch+8.4,6]);
                    translate([3.6,4,-4]) cube([0.4,wswitch+0.4,1]);
                }
            }
        }
        
        // śruby topu
        for (x = [3, mainlen_ex-3]) symetric(1) {
            translate([x-wall,mainwid_ex/2-3,mainh_in-6]) {
                cylinder(d=1.8,h=10);
                cylinder(d=2.4,h=3);
            }
        }
        // otwór na kabel
        translate([pip2[1][0]+pip2[1][1]-wall,mainwid_in/2-1,6]) cube([8,8,20]);
        // otwór na głośnik
        translate([mainlen_in-mi_left,0,-5]) hull() symetric(1) translate([0,(mi_l-mi_d)/2,0]) cylinder(d=mi_d-4,h=7);
        translate([mainlen_in-mi_left,0,-2.1+0.9]) hull() symetric(1) translate([0,(mi_l-mi_d)/2,0]) cylinder(d=mi_d+0.8,h=7);
        
        // otwory na mocowanie głośnika    
        translate([mainlen_in-mi_left,0,-2.1+0.9]) symetric(1) translate([-mi_left+3,mainwid_in/2-3]) hole(mi_h-2.1+0.9);
        // otwory na mocowanie płytki
        for (x=[hlx,hlx+hdx]) symetric(1) translate([x,hdy/2,-2.1+0.9]) hole(2);
        // usb
        translate([-wall-1,-6,4.4]) cube([wall+2,12,5]);
        // klawisz
        translate([xkey-wkey/2,mainwid_in/2-5,mainh_in-wkey]) cube([wkey,8,wkey+1]);
        translate([xkey-wkey/2-2.5,mainwid_in/2-2,mainh_in-wkey]) cube([wkey+5,2,wkey+1]);
        // wyłącznik
        *translate([-wall-1,-wswitch/2,mainh_in-6]) cube([wall+8,wswitch,7.5]);
        *translate([0,-wswitch/2-2.5,mainh_in-7]) cube([2,wswitch+5,8]);
        
        translate([mainlen_in-5,-wswitch/2,mainh_in-6]) cube([wall+8,wswitch,7.5]);
        translate([mainlen_in-2,-wswitch/2-2.5,mainh_in-7]) cube([2,wswitch+5,8]);
    }
}

hprz=5;
module przekladka()
{
    translate([0,0,0.9+hprz+3.6]) mirror([0,0,1]) difference()
    {
        union() {
            translate([20,-mainwid_in/2+1,0]) cube([hlx+hdx-16,mainwid_in -2, 0.9]);
            intersection() {
                for (x=[hlx,hlx+hdx]) symetric(1) translate([x,hdy/2,-0.1]) cylinder(d=4.5, h=0.9+hprz);
                translate([0,-mainwid_in/2+1,0]) cube([hlx+hdx+2,mainwid_in -2, 22.9]);
            }
        }
        translate([hdx+hlx-2.5,-10,-1]) cube([10,20,10]);
        symetric(1) translate([hlx+3,-mainwid_in/2,-1]) cube([hdx-8,6,4]);
        for (x=[hlx,hlx+hdx]) symetric(1) translate([x,hdy/2,-1]) {
            cylinder(d=2.4, h=9.1);
            cylinder(d1=6.4, d2=2.4,h=2);
        }
        // really?
        translate([0,-mainwid_in/2,-1]) cube([hdx+8,6,24]);
    }
}

if (whatprint == 0) buda();
else if (whatprint == 1) topik();
else if (whatprint == 3) przekladka();
else if (whatprint == 2) spikiper();