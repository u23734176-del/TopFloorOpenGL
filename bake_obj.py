import os

def parse_obj(filepath):
    vertices, uvs, normals = [], [], []
    flat_data = [] # Will hold [x, y, z, nx, ny, nz, u, v]

    with open(filepath, 'r') as f:
        for line in f:
            parts = line.strip().split()
            if not parts or parts[0] == '#':
                continue
            
            if parts[0] == 'v':
                vertices.append([float(x) for x in parts[1:4]])
            elif parts[0] == 'vt':
                uvs.append([float(x) for x in parts[1:3]])
            elif parts[0] == 'vn':
                normals.append([float(x) for x in parts[1:4]])
            elif parts[0] == 'f':
                # Grab the face vertices (handles triangles or quads)
                face_verts = parts[1:]
                
                # Triangulate polygon (fan out from the first vertex)
                for i in range(1, len(face_verts) - 1):
                    tri = [face_verts[0], face_verts[i], face_verts[i+1]]
                    
                    for corner in tri:
                        indices = corner.split('/')
                        
                        # Position (OBJ indices are 1-based)
                        v_idx = int(indices[0]) - 1
                        pos = vertices[v_idx]
                        
                        # UVs
                        uv = [0.0, 0.0]
                        if len(indices) > 1 and indices[1] != '':
                            uv_idx = int(indices[1]) - 1
                            uv = uvs[uv_idx]
                            
                        # Normals
                        norm = [0.0, 1.0, 0.0] # Default to UP if missing
                        if len(indices) > 2 and indices[2] != '':
                            n_idx = int(indices[2]) - 1
                            norm = normals[n_idx]
                            
                        flat_data.extend([pos[0], pos[1], pos[2], norm[0], norm[1], norm[2], uv[0], uv[1]])
                        
    return flat_data

def main():
    ext_dir = "external"
    if not os.path.exists(ext_dir):
        print("Could not find external/ directory.")
        return

    for filename in os.listdir(ext_dir):
        if filename.endswith(".obj"):
            print(f"Baking {filename}...")
            data = parse_obj(os.path.join(ext_dir, filename))
            
            # Format as C++ code
            out_filename = filename.replace('.obj', '_data.txt')
            with open(out_filename, 'w') as out:
                out.write(f"// Baked from {filename} | Vertices: {int(len(data)/8)}\n")
                out.write("std::vector<float> vertexData = {\n    ")
                
                for i in range(0, len(data), 8):
                    # Format: x, y, z, nx, ny, nz, u, v
                    line = ", ".join(f"{v}f" for v in data[i:i+8])
                    if i + 8 < len(data):
                        line += ",\n    "
                    out.write(line)
                out.write("\n};\n")
            print(f"Saved to {out_filename}")

if __name__ == "__main__":
    main()