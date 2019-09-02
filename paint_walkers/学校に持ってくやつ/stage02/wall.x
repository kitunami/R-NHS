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
 508;
 0.50000;3.00000;0.50000;,
 -24.50000;3.00000;0.50000;,
 -24.50000;0.00000;0.50000;,
 0.50000;0.00000;0.50000;,
 -24.50000;0.00000;0.50000;,
 -24.50000;3.00000;0.50000;,
 -24.50000;3.00000;-24.50000;,
 -24.50000;0.00000;-24.50000;,
 0.50000;0.00000;0.50000;,
 0.50000;0.00000;-24.50000;,
 0.50000;3.00000;-24.50000;,
 0.50000;3.00000;0.50000;,
 0.50000;3.00000;-24.50000;,
 0.50000;0.00000;-24.50000;,
 -24.50000;0.00000;-24.50000;,
 -24.50000;3.00000;-24.50000;,
 0.50000;1.00000;-3.50000;,
 -1.50000;1.00000;-3.50000;,
 -1.50000;0.00000;-3.50000;,
 0.50000;0.00000;-3.50000;,
 -1.50000;1.00000;-3.50000;,
 -1.50000;1.00000;-2.50000;,
 -1.50000;0.00000;-2.50000;,
 -1.50000;0.00000;-3.50000;,
 -1.50000;1.00000;-2.50000;,
 0.50000;1.00000;-2.50000;,
 0.50000;0.00000;-2.50000;,
 -1.50000;0.00000;-2.50000;,
 0.50000;1.00000;-2.50000;,
 0.50000;1.00000;-3.50000;,
 0.50000;0.00000;-3.50000;,
 0.50000;0.00000;-2.50000;,
 -1.50000;1.00000;-3.50000;,
 0.50000;1.00000;-3.50000;,
 0.50000;0.00000;-3.50000;,
 -1.50000;0.00000;-3.50000;,
 -2.50000;1.00000;-1.50000;,
 -3.50000;1.00000;-1.50000;,
 -3.50000;0.00000;-1.50000;,
 -2.50000;0.00000;-1.50000;,
 -3.50000;1.00000;-1.50000;,
 -3.50000;1.00000;0.50000;,
 -3.50000;0.00000;0.50000;,
 -3.50000;0.00000;-1.50000;,
 -3.50000;1.00000;0.50000;,
 -2.50000;1.00000;0.50000;,
 -2.50000;0.00000;0.50000;,
 -3.50000;0.00000;0.50000;,
 -2.50000;1.00000;0.50000;,
 -2.50000;1.00000;-1.50000;,
 -2.50000;0.00000;-1.50000;,
 -2.50000;0.00000;0.50000;,
 -3.50000;1.00000;-1.50000;,
 -2.50000;1.00000;-1.50000;,
 -2.50000;0.00000;-1.50000;,
 -3.50000;0.00000;-1.50000;,
 -5.50000;1.00000;-3.50000;,
 -6.50000;1.00000;-3.50000;,
 -6.50000;0.00000;-3.50000;,
 -5.50000;0.00000;-3.50000;,
 -6.50000;1.00000;-3.50000;,
 -6.50000;1.00000;-1.50000;,
 -6.50000;0.00000;-1.50000;,
 -6.50000;0.00000;-3.50000;,
 -6.50000;1.00000;-1.50000;,
 -5.50000;1.00000;-1.50000;,
 -5.50000;0.00000;-1.50000;,
 -6.50000;0.00000;-1.50000;,
 -5.50000;1.00000;-1.50000;,
 -5.50000;1.00000;-3.50000;,
 -5.50000;0.00000;-3.50000;,
 -5.50000;0.00000;-1.50000;,
 -6.50000;1.00000;-3.50000;,
 -5.50000;1.00000;-3.50000;,
 -5.50000;0.00000;-3.50000;,
 -6.50000;0.00000;-3.50000;,
 -1.50000;1.00000;-6.50000;,
 -3.50000;1.00000;-6.50000;,
 -3.50000;0.00000;-6.50000;,
 -1.50000;0.00000;-6.50000;,
 -3.50000;1.00000;-6.50000;,
 -3.50000;1.00000;-5.50000;,
 -3.50000;0.00000;-5.50000;,
 -3.50000;0.00000;-6.50000;,
 -3.50000;1.00000;-5.50000;,
 -1.50000;1.00000;-5.50000;,
 -1.50000;0.00000;-5.50000;,
 -3.50000;0.00000;-5.50000;,
 -1.50000;1.00000;-5.50000;,
 -1.50000;1.00000;-6.50000;,
 -1.50000;0.00000;-6.50000;,
 -1.50000;0.00000;-5.50000;,
 -3.50000;1.00000;-6.50000;,
 -1.50000;1.00000;-6.50000;,
 -1.50000;0.00000;-6.50000;,
 -3.50000;0.00000;-6.50000;,
 0.50000;1.00000;-9.50000;,
 -1.50000;1.00000;-9.50000;,
 -1.50000;0.00000;-9.50000;,
 0.50000;0.00000;-9.50000;,
 -1.50000;1.00000;-9.50000;,
 -1.50000;1.00000;-8.50000;,
 -1.50000;0.00000;-8.50000;,
 -1.50000;0.00000;-9.50000;,
 -1.50000;1.00000;-8.50000;,
 0.50000;1.00000;-8.50000;,
 0.50000;0.00000;-8.50000;,
 -1.50000;0.00000;-8.50000;,
 0.50000;1.00000;-8.50000;,
 0.50000;1.00000;-9.50000;,
 0.50000;0.00000;-9.50000;,
 0.50000;0.00000;-8.50000;,
 -1.50000;1.00000;-9.50000;,
 0.50000;1.00000;-9.50000;,
 0.50000;0.00000;-9.50000;,
 -1.50000;0.00000;-9.50000;,
 -3.50000;1.00000;-9.50000;,
 -5.50000;1.00000;-9.50000;,
 -5.50000;0.00000;-9.50000;,
 -3.50000;0.00000;-9.50000;,
 -5.50000;1.00000;-9.50000;,
 -5.50000;1.00000;-8.50000;,
 -5.50000;0.00000;-8.50000;,
 -5.50000;0.00000;-9.50000;,
 -5.50000;1.00000;-8.50000;,
 -3.50000;1.00000;-8.50000;,
 -3.50000;0.00000;-8.50000;,
 -5.50000;0.00000;-8.50000;,
 -3.50000;1.00000;-8.50000;,
 -3.50000;1.00000;-9.50000;,
 -3.50000;0.00000;-9.50000;,
 -3.50000;0.00000;-8.50000;,
 -5.50000;1.00000;-9.50000;,
 -3.50000;1.00000;-9.50000;,
 -3.50000;0.00000;-9.50000;,
 -5.50000;0.00000;-9.50000;,
 -1.50000;1.00000;-15.50000;,
 -3.50000;1.00000;-15.50000;,
 -3.50000;0.00000;-15.50000;,
 -1.50000;0.00000;-15.50000;,
 -3.50000;1.00000;-15.50000;,
 -3.50000;1.00000;-11.50000;,
 -3.50000;0.00000;-11.50000;,
 -3.50000;0.00000;-15.50000;,
 -3.50000;1.00000;-11.50000;,
 -1.50000;1.00000;-11.50000;,
 -1.50000;0.00000;-11.50000;,
 -3.50000;0.00000;-11.50000;,
 -1.50000;1.00000;-11.50000;,
 -1.50000;1.00000;-15.50000;,
 -1.50000;0.00000;-15.50000;,
 -1.50000;0.00000;-11.50000;,
 -3.50000;1.00000;-15.50000;,
 -1.50000;1.00000;-15.50000;,
 -1.50000;0.00000;-15.50000;,
 -3.50000;0.00000;-15.50000;,
 -8.50000;1.00000;-3.50000;,
 -12.50000;1.00000;-3.50000;,
 -12.50000;0.00000;-3.50000;,
 -8.50000;0.00000;-3.50000;,
 -12.50000;1.00000;-3.50000;,
 -12.50000;1.00000;-1.50000;,
 -12.50000;0.00000;-1.50000;,
 -12.50000;0.00000;-3.50000;,
 -12.50000;1.00000;-1.50000;,
 -8.50000;1.00000;-1.50000;,
 -8.50000;0.00000;-1.50000;,
 -12.50000;0.00000;-1.50000;,
 -8.50000;1.00000;-1.50000;,
 -8.50000;1.00000;-3.50000;,
 -8.50000;0.00000;-3.50000;,
 -8.50000;0.00000;-1.50000;,
 -12.50000;1.00000;-3.50000;,
 -8.50000;1.00000;-3.50000;,
 -8.50000;0.00000;-3.50000;,
 -12.50000;0.00000;-3.50000;,
 -1.50000;1.00000;-22.50000;,
 -3.50000;1.00000;-22.50000;,
 -3.50000;0.00000;-22.50000;,
 -1.50000;0.00000;-22.50000;,
 -3.50000;1.00000;-22.50000;,
 -3.50000;1.00000;-20.50000;,
 -3.50000;0.00000;-20.50000;,
 -3.50000;0.00000;-22.50000;,
 -3.50000;1.00000;-20.50000;,
 -1.50000;1.00000;-20.50000;,
 -1.50000;0.00000;-20.50000;,
 -3.50000;0.00000;-20.50000;,
 -1.50000;1.00000;-20.50000;,
 -1.50000;1.00000;-22.50000;,
 -1.50000;0.00000;-22.50000;,
 -1.50000;0.00000;-20.50000;,
 -3.50000;1.00000;-22.50000;,
 -1.50000;1.00000;-22.50000;,
 -1.50000;0.00000;-22.50000;,
 -3.50000;0.00000;-22.50000;,
 -5.50000;1.00000;-22.50000;,
 -6.50000;1.00000;-22.50000;,
 -6.50000;0.00000;-22.50000;,
 -5.50000;0.00000;-22.50000;,
 -6.50000;1.00000;-22.50000;,
 -6.50000;1.00000;-20.50000;,
 -6.50000;0.00000;-20.50000;,
 -6.50000;0.00000;-22.50000;,
 -6.50000;1.00000;-20.50000;,
 -5.50000;1.00000;-20.50000;,
 -5.50000;0.00000;-20.50000;,
 -6.50000;0.00000;-20.50000;,
 -5.50000;1.00000;-20.50000;,
 -5.50000;1.00000;-22.50000;,
 -5.50000;0.00000;-22.50000;,
 -5.50000;0.00000;-20.50000;,
 -6.50000;1.00000;-22.50000;,
 -5.50000;1.00000;-22.50000;,
 -5.50000;0.00000;-22.50000;,
 -6.50000;0.00000;-22.50000;,
 -24.50000;1.00000;-20.50000;,
 -22.50000;1.00000;-20.50000;,
 -22.50000;0.00000;-20.50000;,
 -24.50000;0.00000;-20.50000;,
 -22.50000;1.00000;-20.50000;,
 -22.50000;1.00000;-21.50000;,
 -22.50000;0.00000;-21.50000;,
 -22.50000;0.00000;-20.50000;,
 -22.50000;1.00000;-21.50000;,
 -24.50000;1.00000;-21.50000;,
 -24.50000;0.00000;-21.50000;,
 -22.50000;0.00000;-21.50000;,
 -24.50000;1.00000;-21.50000;,
 -24.50000;1.00000;-20.50000;,
 -24.50000;0.00000;-20.50000;,
 -24.50000;0.00000;-21.50000;,
 -22.50000;1.00000;-20.50000;,
 -24.50000;1.00000;-20.50000;,
 -24.50000;0.00000;-20.50000;,
 -22.50000;0.00000;-20.50000;,
 -21.50000;1.00000;-22.50001;,
 -20.50001;1.00000;-22.50001;,
 -20.50001;0.00000;-22.50001;,
 -21.50000;0.00000;-22.50001;,
 -20.50001;1.00000;-22.50001;,
 -20.50001;1.00000;-24.50001;,
 -20.50001;0.00000;-24.50001;,
 -20.50001;0.00000;-22.50001;,
 -20.50001;1.00000;-24.50001;,
 -21.50000;1.00000;-24.50001;,
 -21.50000;0.00000;-24.50001;,
 -20.50001;0.00000;-24.50001;,
 -21.50000;1.00000;-24.50001;,
 -21.50000;1.00000;-22.50001;,
 -21.50000;0.00000;-22.50001;,
 -21.50000;0.00000;-24.50001;,
 -20.50001;1.00000;-22.50001;,
 -21.50000;1.00000;-22.50001;,
 -21.50000;0.00000;-22.50001;,
 -20.50001;0.00000;-22.50001;,
 -18.50000;1.00000;-20.50000;,
 -17.50000;1.00000;-20.50000;,
 -17.50000;0.00000;-20.50000;,
 -18.50000;0.00000;-20.50000;,
 -17.50000;1.00000;-20.50000;,
 -17.50000;1.00000;-22.50001;,
 -17.50000;0.00000;-22.50001;,
 -17.50000;0.00000;-20.50000;,
 -17.50000;1.00000;-22.50001;,
 -18.50000;1.00000;-22.50001;,
 -18.50000;0.00000;-22.50001;,
 -17.50000;0.00000;-22.50001;,
 -18.50000;1.00000;-22.50001;,
 -18.50000;1.00000;-20.50000;,
 -18.50000;0.00000;-20.50000;,
 -18.50000;0.00000;-22.50001;,
 -17.50000;1.00000;-20.50000;,
 -18.50000;1.00000;-20.50000;,
 -18.50000;0.00000;-20.50000;,
 -17.50000;0.00000;-20.50000;,
 -22.50000;1.00000;-17.49997;,
 -20.50001;1.00000;-17.49997;,
 -20.50001;0.00000;-17.49997;,
 -22.50000;0.00000;-17.49997;,
 -20.50001;1.00000;-17.49997;,
 -20.50001;1.00000;-18.49997;,
 -20.50001;0.00000;-18.49997;,
 -20.50001;0.00000;-17.49997;,
 -20.50001;1.00000;-18.49997;,
 -22.50000;1.00000;-18.49997;,
 -22.50000;0.00000;-18.49997;,
 -20.50001;0.00000;-18.49997;,
 -22.50000;1.00000;-18.49997;,
 -22.50000;1.00000;-17.49997;,
 -22.50000;0.00000;-17.49997;,
 -22.50000;0.00000;-18.49997;,
 -20.50001;1.00000;-17.49997;,
 -22.50000;1.00000;-17.49997;,
 -22.50000;0.00000;-17.49997;,
 -20.50001;0.00000;-17.49997;,
 -24.50000;1.00000;-14.49998;,
 -22.50000;1.00000;-14.49997;,
 -22.50000;0.00000;-14.49997;,
 -24.50000;0.00000;-14.49998;,
 -22.50000;1.00000;-14.49997;,
 -22.50000;1.00000;-15.49997;,
 -22.50000;0.00000;-15.49997;,
 -22.50000;0.00000;-14.49997;,
 -22.50000;1.00000;-15.49997;,
 -24.50000;1.00000;-15.49998;,
 -24.50000;0.00000;-15.49998;,
 -22.50000;0.00000;-15.49997;,
 -24.50000;1.00000;-15.49998;,
 -24.50000;1.00000;-14.49998;,
 -24.50000;0.00000;-14.49998;,
 -24.50000;0.00000;-15.49998;,
 -22.50000;1.00000;-14.49997;,
 -24.50000;1.00000;-14.49998;,
 -24.50000;0.00000;-14.49998;,
 -22.50000;0.00000;-14.49997;,
 -20.50001;1.00000;-14.49997;,
 -18.50000;1.00000;-14.49997;,
 -18.50000;0.00000;-14.49997;,
 -20.50001;0.00000;-14.49997;,
 -18.50000;1.00000;-14.49997;,
 -18.50000;1.00000;-15.49997;,
 -18.50000;0.00000;-15.49997;,
 -18.50000;0.00000;-14.49997;,
 -18.50000;1.00000;-15.49997;,
 -20.50001;1.00000;-15.49997;,
 -20.50001;0.00000;-15.49997;,
 -18.50000;0.00000;-15.49997;,
 -20.50001;1.00000;-15.49997;,
 -20.50001;1.00000;-14.49997;,
 -20.50001;0.00000;-14.49997;,
 -20.50001;0.00000;-15.49997;,
 -18.50000;1.00000;-14.49997;,
 -20.50001;1.00000;-14.49997;,
 -20.50001;0.00000;-14.49997;,
 -18.50000;0.00000;-14.49997;,
 -22.50000;1.00000;-8.49997;,
 -20.50001;1.00000;-8.49997;,
 -20.50001;0.00000;-8.49997;,
 -22.50000;0.00000;-8.49997;,
 -20.50001;1.00000;-8.49997;,
 -20.50001;1.00000;-12.49997;,
 -20.50001;0.00000;-12.49997;,
 -20.50001;0.00000;-8.49997;,
 -20.50001;1.00000;-12.49997;,
 -22.50000;1.00000;-12.49997;,
 -22.50000;0.00000;-12.49997;,
 -20.50001;0.00000;-12.49997;,
 -22.50000;1.00000;-12.49997;,
 -22.50000;1.00000;-8.49997;,
 -22.50000;0.00000;-8.49997;,
 -22.50000;0.00000;-12.49997;,
 -20.50001;1.00000;-8.49997;,
 -22.50000;1.00000;-8.49997;,
 -22.50000;0.00000;-8.49997;,
 -20.50001;0.00000;-8.49997;,
 -15.50000;1.00000;-20.50000;,
 -11.50000;1.00000;-20.50000;,
 -11.50000;0.00000;-20.50000;,
 -15.50000;0.00000;-20.50000;,
 -11.50000;1.00000;-20.50000;,
 -11.50000;1.00000;-22.50001;,
 -11.50000;0.00000;-22.50001;,
 -11.50000;0.00000;-20.50000;,
 -11.50000;1.00000;-22.50001;,
 -15.50000;1.00000;-22.50001;,
 -15.50000;0.00000;-22.50001;,
 -11.50000;0.00000;-22.50001;,
 -15.50000;1.00000;-22.50001;,
 -15.50000;1.00000;-20.50000;,
 -15.50000;0.00000;-20.50000;,
 -15.50000;0.00000;-22.50001;,
 -11.50000;1.00000;-20.50000;,
 -15.50000;1.00000;-20.50000;,
 -15.50000;0.00000;-20.50000;,
 -11.50000;0.00000;-20.50000;,
 -22.50001;1.00000;-1.49997;,
 -20.50001;1.00000;-1.49997;,
 -20.50001;0.00000;-1.49997;,
 -22.50001;0.00000;-1.49997;,
 -20.50001;1.00000;-1.49997;,
 -20.50001;1.00000;-3.49997;,
 -20.50001;0.00000;-3.49997;,
 -20.50001;0.00000;-1.49997;,
 -20.50001;1.00000;-3.49997;,
 -22.50000;1.00000;-3.49997;,
 -22.50000;0.00000;-3.49997;,
 -20.50001;0.00000;-3.49997;,
 -22.50000;1.00000;-3.49997;,
 -22.50001;1.00000;-1.49997;,
 -22.50001;0.00000;-1.49997;,
 -22.50000;0.00000;-3.49997;,
 -20.50001;1.00000;-1.49997;,
 -22.50001;1.00000;-1.49997;,
 -22.50001;0.00000;-1.49997;,
 -20.50001;0.00000;-1.49997;,
 -18.50000;1.00000;-1.49997;,
 -17.50000;1.00000;-1.49997;,
 -17.50000;0.00000;-1.49997;,
 -18.50000;0.00000;-1.49997;,
 -17.50000;1.00000;-1.49997;,
 -17.50000;1.00000;-3.49997;,
 -17.50000;0.00000;-3.49997;,
 -17.50000;0.00000;-1.49997;,
 -17.50000;1.00000;-3.49997;,
 -18.50000;1.00000;-3.49997;,
 -18.50000;0.00000;-3.49997;,
 -17.50000;0.00000;-3.49997;,
 -18.50000;1.00000;-3.49997;,
 -18.50000;1.00000;-1.49997;,
 -18.50000;0.00000;-1.49997;,
 -18.50000;0.00000;-3.49997;,
 -17.50000;1.00000;-1.49997;,
 -18.50000;1.00000;-1.49997;,
 -18.50000;0.00000;-1.49997;,
 -17.50000;0.00000;-1.49997;,
 -10.50000;3.00000;-13.50000;,
 -13.50000;3.00000;-13.50000;,
 -13.50000;0.00000;-13.50000;,
 -10.50000;0.00000;-13.50000;,
 -13.50000;3.00000;-13.50000;,
 -13.50000;3.00000;-10.50000;,
 -13.50000;0.00000;-10.50000;,
 -13.50000;0.00000;-13.50000;,
 -13.50000;3.00000;-10.50000;,
 -10.50000;3.00000;-10.50000;,
 -10.50000;0.00000;-10.50000;,
 -13.50000;0.00000;-10.50000;,
 -10.50000;3.00000;-10.50000;,
 -10.50000;3.00000;-13.50000;,
 -10.50000;0.00000;-13.50000;,
 -10.50000;0.00000;-10.50000;,
 -13.50000;3.00000;-13.50000;,
 -10.50000;3.00000;-13.50000;,
 -10.50000;0.00000;-13.50000;,
 -13.50000;0.00000;-13.50000;,
 0.50000;3.00000;-18.50000;,
 -6.50000;3.00000;-18.50000;,
 -6.50000;0.00000;-18.50000;,
 0.50000;0.00000;-18.50000;,
 -17.50000;3.00000;-5.50000;,
 -17.50000;0.00000;-5.50000;,
 -24.50000;0.00000;-5.50000;,
 -24.50000;3.00000;-5.50000;,
 -18.50000;3.00000;-6.50000;,
 -24.50000;3.00000;-6.50000;,
 -24.50000;0.00000;-6.50000;,
 -18.50000;0.00000;-6.50000;,
 0.50000;3.00000;-17.50000;,
 0.50000;0.00000;-17.50000;,
 -5.50000;0.00000;-17.50000;,
 -5.50000;3.00000;-17.50000;,
 -9.50000;3.00000;-18.50000;,
 -18.50000;3.00000;-18.50000;,
 -18.50000;0.00000;-18.50000;,
 -9.50000;0.00000;-18.50000;,
 -8.50000;3.00000;-17.50000;,
 -8.50000;0.00000;-17.50000;,
 -17.50000;0.00000;-17.50000;,
 -17.50000;3.00000;-17.50000;,
 -9.50000;0.00000;-18.50000;,
 -9.50000;0.00000;-22.50000;,
 -9.50000;3.00000;-22.50000;,
 -9.50000;3.00000;-18.50000;,
 -8.50000;0.00000;-17.50000;,
 -8.50000;3.00000;-17.50000;,
 -8.50000;3.00000;-22.50000;,
 -8.50000;0.00000;-22.50000;,
 -8.50000;3.00000;-22.50000;,
 -9.50000;3.00000;-22.50000;,
 -9.50000;0.00000;-22.50000;,
 -8.50000;0.00000;-22.50000;,
 -14.50000;3.00000;-5.50000;,
 -5.50000;3.00000;-5.50000;,
 -5.50000;0.00000;-5.50000;,
 -14.50000;0.00000;-5.50000;,
 -15.50000;3.00000;-6.50000;,
 -15.50000;0.00000;-6.50000;,
 -6.50000;0.00000;-6.50000;,
 -6.50000;3.00000;-6.50000;,
 -14.50000;0.00000;-5.50000;,
 -14.50000;0.00000;-1.50000;,
 -14.50000;3.00000;-1.50000;,
 -14.50000;3.00000;-5.50000;,
 -15.50000;0.00000;-6.50000;,
 -15.50000;3.00000;-6.50000;,
 -15.50000;3.00000;-1.50000;,
 -15.50000;0.00000;-1.50000;,
 -15.50000;3.00000;-1.50000;,
 -14.50000;3.00000;-1.50000;,
 -14.50000;0.00000;-1.50000;,
 -15.50000;0.00000;-1.50000;,
 -6.50000;0.00000;-6.50000;,
 -6.50000;0.00000;-18.50000;,
 -6.50000;3.00000;-18.50000;,
 -6.50000;3.00000;-6.50000;,
 -5.50000;0.00000;-5.50000;,
 -5.50000;3.00000;-5.50000;,
 -5.50000;3.00000;-17.50000;,
 -5.50000;0.00000;-17.50000;,
 -17.50000;0.00000;-5.50000;,
 -17.50000;3.00000;-5.50000;,
 -17.50000;3.00000;-17.50000;,
 -17.50000;0.00000;-17.50000;,
 -18.50000;0.00000;-6.50000;,
 -18.50000;0.00000;-18.50000;,
 -18.50000;3.00000;-18.50000;,
 -18.50000;3.00000;-6.50000;;
 
 148;
 4;3,2,1,0;,
 4;7,6,5,4;,
 4;11,10,9,8;,
 4;15,14,13,12;,
 4;19,18,17,16;,
 4;23,22,21,20;,
 4;27,26,25,24;,
 4;31,30,29,28;,
 4;33,32,21,28;,
 4;31,22,35,34;,
 4;39,38,37,36;,
 4;43,42,41,40;,
 4;47,46,45,44;,
 4;51,50,49,48;,
 4;53,52,41,48;,
 4;51,42,55,54;,
 4;59,58,57,56;,
 4;63,62,61,60;,
 4;67,66,65,64;,
 4;71,70,69,68;,
 4;73,72,61,68;,
 4;71,62,75,74;,
 4;79,78,77,76;,
 4;83,82,81,80;,
 4;87,86,85,84;,
 4;91,90,89,88;,
 4;93,92,81,88;,
 4;91,82,95,94;,
 4;99,98,97,96;,
 4;103,102,101,100;,
 4;107,106,105,104;,
 4;111,110,109,108;,
 4;113,112,101,108;,
 4;111,102,115,114;,
 4;119,118,117,116;,
 4;123,122,121,120;,
 4;127,126,125,124;,
 4;131,130,129,128;,
 4;133,132,121,128;,
 4;131,122,135,134;,
 4;139,138,137,136;,
 4;143,142,141,140;,
 4;147,146,145,144;,
 4;151,150,149,148;,
 4;153,152,141,148;,
 4;151,142,155,154;,
 4;159,158,157,156;,
 4;163,162,161,160;,
 4;167,166,165,164;,
 4;171,170,169,168;,
 4;173,172,161,168;,
 4;171,162,175,174;,
 4;179,178,177,176;,
 4;183,182,181,180;,
 4;187,186,185,184;,
 4;191,190,189,188;,
 4;193,192,181,188;,
 4;191,182,195,194;,
 4;199,198,197,196;,
 4;203,202,201,200;,
 4;207,206,205,204;,
 4;211,210,209,208;,
 4;213,212,201,208;,
 4;211,202,215,214;,
 4;219,218,217,216;,
 4;223,222,221,220;,
 4;227,226,225,224;,
 4;231,230,229,228;,
 4;233,232,221,228;,
 4;231,222,235,234;,
 4;239,238,237,236;,
 4;243,242,241,240;,
 4;247,246,245,244;,
 4;251,250,249,248;,
 4;253,252,241,248;,
 4;251,242,255,254;,
 4;259,258,257,256;,
 4;263,262,261,260;,
 4;267,266,265,264;,
 4;271,270,269,268;,
 4;273,272,261,268;,
 4;271,262,275,274;,
 4;279,278,277,276;,
 4;283,282,281,280;,
 4;287,286,285,284;,
 4;291,290,289,288;,
 4;293,292,281,288;,
 4;291,282,295,294;,
 4;299,298,297,296;,
 4;303,302,301,300;,
 4;307,306,305,304;,
 4;311,310,309,308;,
 4;313,312,301,308;,
 4;311,302,315,314;,
 4;319,318,317,316;,
 4;323,322,321,320;,
 4;327,326,325,324;,
 4;331,330,329,328;,
 4;333,332,321,328;,
 4;331,322,335,334;,
 4;339,338,337,336;,
 4;343,342,341,340;,
 4;347,346,345,344;,
 4;351,350,349,348;,
 4;353,352,341,348;,
 4;351,342,355,354;,
 4;359,358,357,356;,
 4;363,362,361,360;,
 4;367,366,365,364;,
 4;371,370,369,368;,
 4;373,372,361,368;,
 4;371,362,375,374;,
 4;379,378,377,376;,
 4;383,382,381,380;,
 4;387,386,385,384;,
 4;391,390,389,388;,
 4;393,392,381,388;,
 4;391,382,395,394;,
 4;399,398,397,396;,
 4;403,402,401,400;,
 4;407,406,405,404;,
 4;411,410,409,408;,
 4;413,412,401,408;,
 4;411,402,415,414;,
 4;419,418,417,416;,
 4;423,422,421,420;,
 4;427,426,425,424;,
 4;431,430,429,428;,
 4;433,432,421,428;,
 4;431,422,435,434;,
 4;439,438,437,436;,
 4;443,442,441,440;,
 4;447,446,445,444;,
 4;451,450,449,448;,
 4;455,454,453,452;,
 4;459,458,457,456;,
 4;463,462,461,460;,
 4;467,466,465,464;,
 4;471,470,469,468;,
 4;475,474,473,472;,
 4;479,478,477,476;,
 4;483,482,481,480;,
 4;487,486,485,484;,
 4;491,490,489,488;,
 4;495,494,493,492;,
 4;499,498,497,496;,
 4;503,502,501,500;,
 4;507,506,505,504;;
 
 MeshMaterialList {
  1;
  148;
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
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  11;
  0.000000;0.000000;-1.000000;,
  1.000000;0.000000;0.000000;,
  -1.000000;0.000000;0.000000;,
  -0.000000;0.000000;1.000000;,
  -0.000001;0.000000;1.000000;,
  0.000001;0.000000;-1.000000;,
  -0.000000;0.000000;1.000000;,
  1.000000;0.000000;0.000001;,
  0.000000;1.000000;0.000000;,
  0.000000;-1.000000;0.000000;,
  -1.000000;0.000000;-0.000002;;
  148;
  4;0,0,0,0;,
  4;1,1,1,1;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;4,4,4,4;,
  4;1,1,1,1;,
  4;5,5,5,5;,
  4;2,2,2,2;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;6,6,6,6;,
  4;1,1,1,1;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;3,3,3,3;,
  4;7,7,7,7;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;0,0,0,0;,
  4;10,10,10,10;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;0,0,0,0;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;1,1,1,1;,
  4;8,8,8,8;,
  4;9,9,9,9;,
  4;0,0,0,0;,
  4;3,3,3,3;,
  4;0,0,0,0;,
  4;3,3,3,3;,
  4;0,0,0,0;,
  4;3,3,3,3;,
  4;2,2,2,2;,
  4;1,1,1,1;,
  4;0,0,0,0;,
  4;3,3,3,3;,
  4;0,0,0,0;,
  4;1,1,1,1;,
  4;2,2,2,2;,
  4;3,3,3,3;,
  4;2,2,2,2;,
  4;1,1,1,1;,
  4;1,1,1,1;,
  4;2,2,2,2;;
 }
 MeshTextureCoords {
  508;
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  0.000000;1.000000;,
  1.000000;1.000000;,
  1.000000;0.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;;
 }
}
