xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 12;
 0.00000;0.00000;0.41374;,
 0.11682;0.00000;0.10981;,
 0.00000;0.10671;-0.00177;,
 -0.11682;0.00000;0.10981;,
 0.28034;0.00000;-0.40233;,
 0.00000;0.00000;-0.16192;,
 0.16529;0.00000;-0.08252;,
 0.42762;0.00000;0.11059;,
 -0.42762;0.00000;0.11059;,
 -0.16529;0.00000;-0.08252;,
 -0.28034;0.00000;-0.40233;,
 0.00000;-0.10671;-0.00177;;
 
 20;
 3;0,1,2;,
 3;2,3,0;,
 3;4,5,2;,
 3;4,2,6;,
 3;7,6,2;,
 3;7,2,1;,
 3;8,3,2;,
 3;8,2,9;,
 3;10,9,2;,
 3;10,2,5;,
 3;0,11,1;,
 3;11,0,3;,
 3;4,11,5;,
 3;4,6,11;,
 3;7,11,6;,
 3;7,1,11;,
 3;8,11,3;,
 3;8,9,11;,
 3;10,11,9;,
 3;10,5,11;;
 
 MeshMaterialList {
  1;
  20;
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0;;
  Material {
   0.992000;1.000000;0.125000;1.000000;;
   17.000000;
   1.000000;1.000000;1.000000;;
   0.248000;0.250000;0.031250;;
  }
 }
 MeshNormals {
  34;
  0.291097;0.825063;0.484287;,
  0.543302;0.813150;0.208829;,
  0.324691;0.836678;-0.441074;,
  0.496324;0.859769;-0.120248;,
  -0.429441;0.751540;-0.500768;,
  0.000000;0.832182;-0.554502;,
  -0.291097;0.825063;0.484287;,
  0.001753;0.721775;0.692126;,
  -0.496324;0.859769;-0.120248;,
  -0.601985;0.768580;0.216562;,
  0.229983;0.954024;0.192213;,
  0.229983;-0.954024;0.192213;,
  -0.459079;0.849691;-0.259367;,
  -0.543302;0.813150;0.208829;,
  0.601985;0.768580;0.216562;,
  -0.001753;0.721775;0.692126;,
  -0.324691;0.836678;-0.441074;,
  0.429441;0.751540;-0.500768;,
  0.543302;-0.813150;0.208829;,
  0.291097;-0.825063;0.484287;,
  -0.459079;-0.849691;-0.259367;,
  -0.543302;-0.813150;0.208829;,
  -0.291097;-0.825063;0.484287;,
  -0.429441;-0.751540;-0.500768;,
  0.000000;-0.832182;-0.554502;,
  0.601985;-0.768580;0.216562;,
  0.496324;-0.859769;-0.120248;,
  0.324691;-0.836678;-0.441074;,
  -0.001753;-0.721775;0.692126;,
  0.001753;-0.721775;0.692126;,
  -0.324691;-0.836678;-0.441074;,
  -0.496324;-0.859769;-0.120248;,
  -0.601985;-0.768580;0.216562;,
  0.429441;-0.751540;-0.500768;;
  20;
  3;1,0,10;,
  3;12,6,13;,
  3;4,5,12;,
  3;14,10,3;,
  3;2,3,10;,
  3;15,10,0;,
  3;7,6,10;,
  3;16,12,8;,
  3;9,8,10;,
  3;17,10,5;,
  3;18,11,19;,
  3;20,21,22;,
  3;23,20,24;,
  3;25,26,11;,
  3;27,11,26;,
  3;28,19,11;,
  3;29,11,22;,
  3;30,31,20;,
  3;32,11,31;,
  3;33,24,11;;
 }
 MeshTextureCoords {
  12;
  0.000000;0.000000;
  0.000000;0.000000;
  0.000000;0.000000;
  0.000000;0.000000;
  0.000000;0.000000;
  0.000000;0.000000;
  0.000000;0.000000;
  0.000000;0.000000;
  0.000000;0.000000;
  0.000000;0.000000;
  0.000000;0.000000;
  0.000000;0.000000;;
 }
}
