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

template FrameTransformMatrix {
 <F6F23F41-7686-11cf-8F52-0040333594A3>
 Matrix4x4 frameMatrix;
}

template Frame {
 <3D82AB46-62DA-11cf-AB39-0020AF71E433>
 [...]
}

Header {
 1;
 0;
 1;
}

Mesh Body {
 24;
 -0.42;0.74;-0.04;,
 0.42;0.74;-0.04;,
 0.50;0.88;-0.08;,
 -0.50;0.88;-0.08;,
 0.84;0.00;-0.04;,
 1.00;0.00;-0.08;,
 -0.42;0.74;0.04;,
 0.42;0.74;0.04;,
 0.84;0.00;0.04;,
 -1.00;0.00;-0.08;,
 -0.84;0.00;-0.04;,
 -0.84;0.00;0.04;,
 -0.50;-0.88;-0.08;,
 0.50;-0.88;-0.08;,
 0.42;-0.74;-0.04;,
 -0.42;-0.74;-0.04;,
 0.42;-0.74;0.04;,
 -0.42;-0.74;0.04;,
 -0.50;0.88;0.08;,
 0.50;0.88;0.08;,
 1.00;0.00;0.08;,
 -1.00;0.00;0.08;,
 0.50;-0.88;0.08;,
 -0.50;-0.88;0.08;;

  24;
 4;3,2,1,0;,
 4;1,2,5,4;,
 4;7,6,0,1;,
 4;7,1,4,8;,
 4;10,9,3,0;,
 4;11,10,0,6;,
 4;15,14,13,12;,
 4;4,5,13,14;,
 4;14,15,17,16;,
 4;8,4,14,16;,
 4;15,12,9,10;,
 4;17,15,10,11;,
 4;6,7,19,18;,
 4;8,20,19,7;,
 4;6,18,21,11;,
 4;23,22,16,17;,
 4;16,22,20,8;,
 4;11,21,23,17;,
 4;12,23,21,9;,
 4;13,22,23,12;,
 4;5,20,22,13;,
 4;2,19,20,5;,
 4;3,18,19,2;,
 4;9,21,18,3;;

 MeshMaterialList {
  3;
  24;
  0,
  0,
  1,
  2,
  0,
  2,
  0,
  0,
  2,
  1,
  0,
  1,
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
  0;

  Material {
   0.65;0.65;0.65;1.0;;
   99.00;;
   0.70;0.70;0.70;;
   0.00;0.00;0.00;;
  }
  Material {
   0.00;0.00;0.00;1.0;;
   100.00;;
   0.00;0.00;0.00;;
   1.00;1.00;1.00;;
  }
  Material {
   0.00;0.00;0.00;1.0;;
   100.00;;
   0.00;0.00;0.00;;
   0.05;0.10;0.20;;
  }
 }

 MeshNormals {
  22;
  0.00;-0.274721;-0.961524;,
  -0.240610;-0.136643;-0.960956;,
  0.00;-1.00;-0.00;,
  -0.869686;-0.493606;-0.00;,
  0.240610;-0.136643;-0.960956;,
  0.869686;-0.493606;-0.00;,
  0.00;0.274721;-0.961524;,
  -0.240610;0.136643;-0.960956;,
  0.00;1.00;-0.00;,
  -0.869686;0.493606;-0.00;,
  0.240610;0.136643;-0.960956;,
  0.869686;0.493606;-0.00;,
  0.00;-0.274721;0.961524;,
  -0.240610;-0.136643;0.960956;,
  0.240610;-0.136643;0.960956;,
  0.00;0.274721;0.961524;,
  -0.240610;0.136643;0.960956;,
  0.240610;0.136643;0.960956;,
  -0.869457;-0.494009;-0.00;,
  0.869457;-0.494009;-0.00;,
  0.869457;0.494009;-0.00;,
  -0.869457;0.494009;-0.00;;

  24;
  4;0,0,0,0;,
  4;1,1,1,1;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;4,4,4,4;,
  4;5,5,5,5;,
  4;6,6,6,6;,
  4;7,7,7,7;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;10,10,10,10;,
  4;11,11,11,11;,
  4;12,12,12,12;,
  4;13,13,13,13;,
  4;14,14,14,14;,
  4;15,15,15,15;,
  4;16,16,16,16;,
  4;17,17,17,17;,
  4;18,18,18,18;,
  4;2,2,2,2;,
  4;19,19,19,19;,
  4;20,20,20,20;,
  4;8,8,8,8;,
  4;21,21,21,21;;
 }
}
