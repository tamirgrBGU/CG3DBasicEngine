#version 330

in vec2 texcoord0;
in vec4 position0;

out vec4 color;


uniform float r1r;
uniform float r1i;
uniform float r2r;
uniform float r2i;
uniform float r3r;
uniform float r3i;
uniform float r4r;
uniform float r4i;
uniform float a;
uniform float b;
uniform float c;
uniform float d;
uniform float e;
uniform float k;
uniform float r;
uniform float g;
uniform float b1;
uniform vec4 move;
uniform float zoom;
uniform int isQuartic;




vec2 add(vec2 e1, vec2 e2){
    float r = e1[0]+e2[0];
    float i = e1[1]+e2[1];
    return vec2(r,i);
}

vec2 sub(vec2 e1,vec2 e2){
    float r = e1.x-e2.x;
    float i = e1.y-e2.y;
    vec2 res = vec2(r,i);
    return res;
}


vec2 mult(vec2 e1, vec2 e2){
    float r = (e1[0]*e2[0])-(e1[1]*e2[1]);
    float i = (e1[0]*e2[1])+(e1[1]*e2[0]);
    return vec2(r,i);
}

vec2 divide(vec2 e1, vec2 e2){
    float r = ((e1[0]*e2[0])+(e1[1]*e2[1]))/((e2[0]*e2[0])+(e2[1]*e2[1]));
    float i = ((e1[1]*e2[0])-(e1[0]*e2[1]))/((e2[0]*e2[0])+(e2[1]*e2[1]));
    return vec2(r,i);
}


vec2 pow(vec2 num, int p){

    if(p == 0){
        return vec2(1.0,0.0);
    }
    vec2 res = vec2(1.0, 0.0);
    for(int i=0;i<p;i++){
        res = mult(res,num);
    }
    return res;
}

vec2 sum(vec2[5] nums){
    vec2 res = vec2(0.0,0.0);
    for(int i=0 ; i<5 ; i++){
        res = add(res,nums[i]);

    }
    return res;
}


vec2 fCubic(vec2 num){
    vec2 first = mult(vec2(a,0.0),pow(num,3));
    vec2 second = mult(vec2(b,0.0),pow(num,2));
    vec2 third = mult(vec2(c,0.0),num);
    vec2 fourth = vec2(d,0.0);
    vec2[5] arr = vec2[5](first,second,third,fourth,vec2(0.0,0.0));
    return sum(arr);
}

vec2 derCubic(vec2 num){
    vec2 first = mult(vec2(3.0*a,0.0),pow(num,2));
    vec2 second = mult(vec2(2.0*b,0.0),num);
    vec2 third = vec2(c,0.0);
    vec2[5] arr = vec2[5](first,second,third,vec2(0.0,0.0),vec2(0.0,0.0));
    return sum(arr);

}

vec2 fQuartic(vec2 num){
    vec2 first = mult(vec2(a,0.0),pow(num,4));
    vec2 second = mult(vec2(b,0.0),pow(num,3));
    vec2 third = mult(vec2(c,0.0),pow(num,2));
    vec2 fourth = mult(vec2(d,0.0),num);
    vec2 fifth = vec2(e,0.0);
    vec2[5] arr = vec2[5](first,second,third,fourth,fifth);
    return sum(arr);
}

vec2 derQuartic(vec2 num){
    vec2 first = mult(vec2(4.0*a,0.0),pow(num,3));
    vec2 second = mult(vec2(3.0*b,0.0),pow(num,2));
    vec2 third = mult(vec2(2.0*c,0.0),num);
    vec2 fourth = vec2(d,0.0);
    vec2 fifth = vec2(0.0,0.0);
    vec2[5] arr = vec2[5](first,second,third,fourth,fifth);
    return sum(arr);
}

float dist(vec2 e1, vec2 e2){
    float x = e1[0]-e2[0];
    float y = e1[1]-e2[1];
    return sqrt((x*x)+(y*y));

}



int findClosestRoot(vec2 num){
    vec2 r1 = vec2(r1r,r1i);
    vec2 r2 = vec2(r2r,r2i);
    vec2 r3 = vec2(r3r,r3i);
    float dist1 = dist(r1,num);
    float dist2 = dist(r2,num);
    float dist3 = dist(r3,num);

    if(dist1<dist2 && dist1<dist3){
        return 0;
    }else if(dist2<dist1 && dist2<dist3){
        return 1;
    }else if(dist3<dist1 && dist3<dist2){
        return 2;
    }
    return 3;
}

int findClosestRootQurtic(vec2 num){
    vec2 r1 = vec2(r1r,r1i);
    vec2 r2 = vec2(r2r,r2i);
    vec2 r3 = vec2(r3r,r3i);
    vec2 r4 = vec2(r4r,r4i);
    float dist1 = dist(r1,num);
    float dist2 = dist(r2,num);
    float dist3 = dist(r3,num);
    float dist4 = dist(r4,num);

    if(dist1<dist2 && dist1<dist3 && dist1<dist4){
        return 0;
    }else if(dist2<dist1 && dist2<dist3 && dist2<dist4){
        return 1;
    }else if(dist3<dist1 && dist3<dist2 && dist3<dist4){
        return 2;
    }
    else if(dist4<dist1 && dist4<dist2 && dist4<dist3){
        return 3;
    }
    return 3;
}


void main()
{
    vec4[4] colors = vec4[4](vec4(r,0.0,0.0,1.0),vec4(0.0,g,0.0,1.0),vec4(0.0,0.0,b1,1.0),vec4(1.0,0.0,1.0,1.0));
    vec2 z = vec2((position0.x - move[0]) * zoom,(position0.y + move[1]) * zoom);
    int index;

    if(isQuartic == 1){
        for(int i = 0; i < k; i++){
            vec2 fz = fQuartic(z);
            vec2 dz = derQuartic(z);
            vec2 x = divide(fz,dz);
            vec2 neg = vec2((-1)*x[0],(-1)*x[1]);
            z = add(z,neg);
        }
        index  = findClosestRootQurtic(z);
    }else{
        for(int i =0; i<k ; i++){
            vec2 fz = fCubic(z);
            vec2 dz = derCubic(z);
            vec2 x = divide(fz,dz);
            vec2 neg = vec2((-1)*x[0],(-1)*x[1]);
            z = add(z,neg);
        }
        index = findClosestRoot(z);
    }


    color = vec4(colors[index]);
}