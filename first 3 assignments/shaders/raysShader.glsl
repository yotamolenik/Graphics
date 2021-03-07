#version 130 

uniform vec4 eye;
uniform vec4 ambient;
uniform vec4[20] objects;
uniform vec4[20] objColors;
uniform vec4[10] lightsDirection;
uniform vec4[10] lightsIntensity;
uniform vec4[10] lightPosition;
uniform ivec4 sizes; //{number of objects , number of lights , width, hight}  

in vec3 position1;

const float delta = 0.0001;

float mapRange(float num, float total, float minR, float maxR) {
	return num * ((maxR - minR) / total) + minR;
}

float intersectionSphere(vec4 sphere, vec3 sourcePoint, vec3 v){
	vec3 src_obj = sourcePoint - sphere.xyz;
	float a = 1.0;
	float b = 2.0 * dot(v, src_obj);
	float c = dot(src_obj, src_obj) - sphere.w * sphere.w;
	float d = b * b - 4.0 * a * c;
	if (d < 0.0) {}// no intersection
	else if ((-b - sqrt(d)) / (2.0 * a) > 0) // line intersects sphere	
		return (-b - sqrt(d)) / (2.0 * a);
	return 0;
}
float intersectionPlane(vec4 plane, vec3 sourcePoint, vec3 v){
	vec3 N = normalize(plane.xyz);
	vec3 p0 = vec3(0, 0, -plane.w / plane.z);
	if(dot(v, N) < delta)
		return 0;
	float t = dot(p0 - sourcePoint, N) / dot(v, N);
	return max(t, 0.0);
}
vec4 intersection(vec3 sourcePoint, vec3 v, int skipped) {
	float tMin = 0;
	float index = -1;
	for (int k = 0; k < sizes[0]; k++) {
		if (skipped == k)
			continue;
		float t = 0;
		if (objects[k].w < 0) { //plane	
			t = intersectionPlane(objects[k], sourcePoint, v);
		}
		else {
			t = intersectionSphere(objects[k], sourcePoint, v);
		}
		if (t > 0 && (tMin == 0.0 || tMin > t)) {
			tMin = t;
			index= k;
		}
	}
	return vec4(sourcePoint + tMin*v, index);
}
int getIntersectingObj(vec3 in_ray, vec3 hit) {
	for (int i = 0; i < sizes[0]; i++) {
		if (objects[i].w < 0) {
			if (dot(normalize(objects[i].xyz), in_ray) > delta)
				return i;
		}
		else if (dot(hit - objects[i].xyz, hit - objects[i].xyz) - (objects[i].w * objects[i].w) < delta) {
			return i;
		}
	}
	return -1;
}

float calcdiffuse(vec3 N, vec3 D){
	return max(dot(N, normalize(D)), 0.0);
}
float calcspecular(vec3 N, vec3 D,  vec3 viewDir, float n){
	vec3 reflectDir = reflect(-normalize(D), N);
	float spec = max(dot(viewDir, reflectDir), 0.0);
	return 0.7 * pow(spec, n);
}

bool calcspotlight(vec4 P, vec3 D, vec3 hit) { 
	float theta = dot(normalize(hit- P.xyz), normalize(D));
	if(degrees(acos(theta)) > 180 || acos(theta) < 0){
		return false;
	}
	return (theta > P.w);
}
bool shadow(bool spotLight, vec4 hit, int light) {
	vec3 V = -normalize(spotLight ? (hit.xyz - lightPosition[light].xyz): lightsDirection[light].xyz);
	vec4 nextHit = intersection(hit.xyz, V, int(hit.w));
	float t = length(nextHit.xyz);
	int index = int(nextHit.w);
	if(index > -1 && !(objects[index].w < 0.0)) //hit sphere
		return t > 0 && t < length(hit.xyz - lightPosition[light].xyz);
	return false;
}
vec3 calcLight(int light, vec3 N, vec3 V, vec4 hit, float n, bool checkered){
	vec3 D = lightsDirection[light].xyz;
	float Kd = (checkered ? 0.5 : 1) * calcdiffuse(N, D);
	float Ks =  2 * calcspecular(N, D, V, n);
	vec3 color = (Kd + Ks) * lightsIntensity[light].xyz;
	if(!shadow(lightsDirection[light].w == 1.0, hit, light)){
		if (lightsDirection[light].w == 1.0 && 
			!calcspotlight(lightPosition[light], D, hit.xyz)){} //spotlight but point not affected
		else
			return color;
	}
	return vec3(0,0,0);
}

vec3 colorCalc(vec4 hit, vec3 V, vec3 N){
	vec3 color = vec3(0, 0, 0);
	int index = int(hit.w);
	bool checkered = mod(int(1.5 * hit.x + 100), 2) == mod(int(1.5 * hit.y + 100), 2);
	for (int i = 0; i < sizes[1]; i++) {
		color += objColors[index].xyz *
			calcLight(i, N, V, hit, objColors[index].w, objects[index].w < 0 && checkered);
	}
	return color;
}
vec3 colorCalc(vec3 intersectionPoint){
    vec3 color = 0.25 * ambient.xyz;
	vec3 V = normalize(vec3(position1.xy, 0) - intersectionPoint);
	vec4 hit = intersection(intersectionPoint, V, -1);
	
	int index = int(hit.w);
	vec3 N = normalize(objects[index].w < 0.0 ? objects[index].xyz : (objects[index].xyz -hit.xyz));
	if(index >= sizes[2]){ //not reflection
		color += colorCalc(hit, V, N);
	}
	else if(index > -1){ //reflection obj
		for (int j = 0; j < 2; j++) {
			V = reflect(normalize(V), normalize(N));
			hit = intersection(hit.xyz, V, -1);
			
			index = int(hit.w);
			N = normalize(objects[index].w < 0.0 ? objects[index].xyz : (objects[index].xyz -hit.xyz));
			if(index >= sizes[2]){
				color += colorCalc(hit, V, N);
			} //colored
			else if(hit.w > -1){
				j--;
			} //reflective
			else
				break;
		}
	}
    return clamp(color, 0, 1);;
}

vec3 calcColor2(vec3 intersectionPoint, vec3 inV, vec3 N){
	vec3 color = vec3(0,0,0);
	vec3 V = reflect(normalize(inV), normalize(N));
	float t = 0; //intersection(intersectionPoint, V, -1);
	if (t > 0.0){
		vec3 hit = intersectionPoint + t * V;
		int index = getIntersectingObj(V, hit);
		vec3 N = normalize(objects[index].w < 0.0 ? objects[index].xyz : (objects[index].xyz -hit));
		bool sq = mod(int(1.5 * hit.x + 100), 2) == mod(int(1.5 * hit.y + 100), 2);
		vec3 Kd = objColors[index].xyz;
//		for (int i = 0; i < sizes[1]; i++) {
//			vec3 c = Kd * calcLight(N, lightsDirection[i].xyz, lightsIntensity[i].xyz, 
//				V, objColors[index].w, objects[index].w < 0 && sq ? 1/2 : 1);
//			if (lightsDirection[i].w == 1.0 ){
//				if(calcspotlight(hit, lightPosition[i].xyz, lightsDirection[i].xyz, lightPosition[i].w)) {
//					color += c/2;
//				}
//			}
//			else if (!shadow(lightsDirection[i].w == 1.0, hit, index, i))
//				color += c;
//		}
	}
	return color;
}

void main()
{  
	gl_FragColor = vec4(colorCalc(eye.xyz),1);
}