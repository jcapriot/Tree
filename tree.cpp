#include <vector>
#include <map>
#include "tree.h"
#include <iostream>

Node::Node(){
    location[0] = 0;
    location[1] = 0;
    location[2] = 0;
    key = 0;
    reference = 0;
    index = 0;
    hanging = false;
    parents[0] = NULL;
    parents[1] = NULL;
    parents[2] = NULL;
    parents[3] = NULL;
};

Node::Node(int_t x, int_t y, int_t z){
    location[0] = x;
    location[1] = y;
    location[2] = z;
    key = key_func(x, y, z);
    reference = 0;
    hanging = false;
    parents[0] = NULL;
    parents[1] = NULL;
    parents[2] = NULL;
    parents[3] = NULL;
}

Edge::Edge(){
    location[0] = 0;
    location[1] = 0;
    location[2] = 0;
    key = 0;
    index = 0;
    reference = 0;
    hanging = false;
    points[0] = NULL;
    points[1] = NULL;
    parents[0] = NULL;
    parents[1] = NULL;
}

Edge::Edge(Node& p1, Node& p2){
      points[0] = &p1;
      points[1] = &p2;
      int_t x, y, z;
      x = (p1[0]+p2[0])/2;
      y = (p1[1]+p2[1])/2;
      z = (p1[2]+p2[2])/2;
      key = key_func(x, y, z);
      location[0] = x;
      location[1] = y;
      location[2] = z;
      length = (p2[0]-p1[0])+(p2[1]-p1[1])+(p2[2]-p1[2]);
      reference = 0;
      hanging = false;
      parents[0] = NULL;
      parents[1] = NULL;
}

Face::Face(){
    location[0] = 0;
    location[1] = 0;
    location[2] = 0;
    key = 0;
    reference = 0;
    index = 0;
    area = 0;
    hanging = false;
    points[0] = NULL;
    points[1] = NULL;
    points[2] = NULL;
    points[3] = NULL;
    edges[0] = NULL;
    edges[1] = NULL;
    edges[2] = NULL;
    edges[3] = NULL;
    parent = NULL;
}

Face::Face(Node& p1, Node& p2, Node& p3, Node& p4){
    points[0]= &p1;
    points[1]= &p2;
    points[2]= &p3;
    points[3]= &p4;
    int_t x, y, z;
    x = (p1[0]+p2[0]+p3[0]+p4[0])/4;
    y = (p1[1]+p2[1]+p3[1]+p4[1])/4;
    z = (p1[2]+p2[2]+p3[2]+p4[2])/4;
    key = key_func(x, y, z);
    location[0] = x;
    location[1] = y;
    location[2] = z;
    int_t length = (p2[0]-p1[0])+(p2[1]-p1[1])+(p2[2]-p1[1]);
    area = length*length;
    reference = 0;
    hanging = false;
    parent = NULL;
    edges[0] = NULL;
    edges[1] = NULL;
    edges[2] = NULL;
    edges[3] = NULL;

}

Node * set_default_node(node_map_t& nodes, int_t x, int_t y, int_t z){
  int_t key = key_func(x, y, z);
  Node * point;
  if(nodes.count(key)==0){
    point = new Node(x, y, z);
    nodes[key] = point;
  }
  else{
    point = nodes[key];
  }
  return point;
}

Edge * set_default_edge(edge_map_t& edges, Node& p1, Node& p2){
  int_t xC = (p1[0]+p2[0])/2;
  int_t yC = (p1[1]+p2[1])/2;
  int_t zC = (p1[2]+p2[2])/2;
  int_t key = key_func(xC, yC, zC);
  Edge * edge;
  if(edges.count(key)==0){
    edge = new Edge(p1, p2);
    edges[key] = edge;
  }
  else{
    edge = edges[key];
  }
  return edge;
};

Face * set_default_face(face_map_t& faces, Node& p1, Node& p2, Node& p3, Node& p4){
    int_t x, y, z, key;
    x = (p1[0]+p2[0]+p3[0]+p4[0])/4;
    y = (p1[1]+p2[1]+p3[1]+p4[1])/4;
    z = (p1[2]+p2[2]+p3[2]+p4[2])/4;
    key = key_func(x, y, z);
    Face * face;
    if(faces.count(key)==0){
        face = new Face(p1, p2, p3, p4);
        faces[key] = face;
    }
    else{
        face = faces[key];
    }
    return face;
}

Cell::Cell(Node *pts[8], int_t ndim, int_t maxlevel, function func){
    n_dim = ndim;
    int_t n_points = 1<<n_dim;
    for(int_t i=0; i<n_points; ++i)
        points[i] = pts[i];
    level = 0;
    max_level = maxlevel;
    parent = NULL;
    test_func = func;
    Node p1 = *pts[0];
    Node p2 = *pts[n_points-1];
    center[0] = (p1[0]+p2[0])/2;
    center[1] = (p1[1]+p2[1])/2;
    center[2] = (p1[2]+p2[2])/2;
    key = key_func(center[0],center[1],center[2]);
    for(int_t i=0; i<n_points; ++i)
        children[i] = NULL;
    for(int_t i=0; i<2*n_dim; ++i)
        neighbors[i] = NULL;
};

Cell::Cell(Node *pts[8], Cell *parent){
    n_dim = parent->n_dim;
    int_t n_points = 1<<n_dim;
    for(int_t i=0; i<n_points; ++i)
        points[i] = pts[i];
    level = parent->level+1;
    max_level = parent->max_level;
    test_func = parent->test_func;
    Node p1 = *pts[0];
    Node p2 = *pts[n_points-1];
    center[0] = (p1[0]+p2[0])/2;
    center[1] = (p1[1]+p2[1])/2;
    center[2] = (p1[2]+p2[2])/2;
    key = key_func(center[0], center[1], center[2]);

    for(int_t i=0; i<n_points; ++i)
        children[i] = NULL;
    for(int_t i=0; i<2*n_dim; ++i)
        neighbors[i] = NULL;
};

void Cell::spawn(node_map_t& nodes, Cell *kids[8]){
    /*      z0              z0+dz/2          z0+dz
        p03--p13--p04    p20--p21--p22   p07--p27--p08
        |     |    |     |     |    |    |     |    |
        p10--p11--p12    p17--p18--p19   p24--p25--p26
        |     |    |     |     |    |    |     |    |
        p01--p09--p02    p14--p15--p16   p05--p23--p06
    */
    Node *p1 = points[0];
    Node *p2 = points[1];
    Node *p3 = points[2];
    Node *p4 = points[3];

    int_t x0, y0, xC, yC, xF, yF, z0;

    x0 = p1->location[0];
    y0 = p1->location[1];
    xF = p4->location[0];
    yF = p4->location[1];
    z0 = p1->location[2];
    xC = center[0];
    yC = center[1];

    Node *p9, *p10, *p11, *p12, *p13;
    p9  = set_default_node(nodes, xC, y0, z0);
    p10 = set_default_node(nodes, x0, yC, z0);
    p11 = set_default_node(nodes, xC, yC, z0);
    p12 = set_default_node(nodes, xF, yC, z0);
    p13 = set_default_node(nodes, xC, yF, z0);

    if(n_dim>2){
        Node *p5 = points[4];
        Node *p6 = points[5];
        Node *p7 = points[6];
        Node *p8 = points[7];

        int_t zC, zF;

        zF = p8->location[2];
        zC = center[2];

        Node *p14, *p15, *p16, *p17, *p18, *p19, *p20, *p21, *p22;
        Node *p23, *p24, *p25, *p26, *p27;

        p14 = set_default_node(nodes, x0, y0, zC);
        p15 = set_default_node(nodes, xC, y0, zC);
        p16 = set_default_node(nodes, xF, y0, zC);
        p17 = set_default_node(nodes, x0, yC, zC);
        p18 = set_default_node(nodes, xC, yC, zC);
        p19 = set_default_node(nodes, xF, yC, zC);
        p20 = set_default_node(nodes, x0, yF, zC);
        p21 = set_default_node(nodes, xC, yF, zC);
        p22 = set_default_node(nodes, xF, yF, zC);

        p23 = set_default_node(nodes, xC, y0, zF);
        p24 = set_default_node(nodes, x0, yC, zF);
        p25 = set_default_node(nodes, xC, yC, zF);
        p26 = set_default_node(nodes, xF, yC, zF);
        p27 = set_default_node(nodes, xC, yF, zF);

        Node * pQC1[8] = {p1,p9,p10,p11,p14,p15,p17,p18};
        Node * pQC2[8] = {p9,p2,p11,p12,p15,p16,p18,p19};
        Node * pQC3[8] = {p10,p11,p3,p13,p17,p18,p20,p21};
        Node * pQC4[8] = {p11,p12,p13,p4,p18,p19,p21,p22};
        Node * pQC5[8] = {p14,p15,p17,p18,p5,p23,p24,p25};
        Node * pQC6[8] = {p15,p16,p18,p19,p23,p6,p25,p26};
        Node * pQC7[8] = {p17,p18,p20,p21,p24,p25,p7,p27};
        Node * pQC8[8] = {p18,p19,p21,p22,p25,p26,p27,p8};

        kids[0] = new Cell(pQC1,this);
        kids[1] = new Cell(pQC2,this);
        kids[2] = new Cell(pQC3,this);
        kids[3] = new Cell(pQC4,this);
        kids[4] = new Cell(pQC5,this);
        kids[5] = new Cell(pQC6,this);
        kids[6] = new Cell(pQC7,this);
        kids[7] = new Cell(pQC8,this);
    }
    else{
        Node * pQC1[8] = {p1,p9,p10,p11,NULL,NULL,NULL,NULL};
        Node * pQC2[8] = {p9,p2,p11,p12,NULL,NULL,NULL,NULL};
        Node * pQC3[8] = {p10,p11,p3,p13,NULL,NULL,NULL,NULL};
        Node * pQC4[8] = {p11,p12,p13,p4,NULL,NULL,NULL,NULL};
        kids[0] = new Cell(pQC1,this);
        kids[1] = new Cell(pQC2,this);
        kids[2] = new Cell(pQC3,this);
        kids[3] = new Cell(pQC4,this);
    }
};

void Cell::set_neighbor(Cell * other, int_t position){
    if(other==NULL){
        //std::cout<<"Other cell was NULL"<<std::endl;
        return;
    }
    //std::cout<<"Making cells "<<center[0]<<","<<center[1];
    //std::cout<<" and "<< other->center[0]<<","<<other->center[1];
    //std::cout<<" neighbors"<<std::endl;
    if(level != other->level){
        neighbors[position] = other;
    }else{
        neighbors[position] = other;
        other->neighbors[position^1] = this;
    }//else{
     //   neighbors[position] = other;
     //   other->neighbors[position^1] = parent;
    //}
};

void Cell::divide(node_map_t& nodes, bool force=false){
    bool do_splitting = false;
    if(level==max_level){
        do_splitting = false;
    }else if(force){
        do_splitting = true;
    }else{
        int_t test_level = (*test_func)(this);
        if(test_level > level){
            do_splitting = true;
        }
    }
    if(!do_splitting){
        return;
    }
    //If i haven't already been split...
    if(children[0]==NULL){
        spawn(nodes, children);

        //If I need to be split, and my neighbor is below my level
        //Then it needs to be split
        //-x,+x,-y,+y,-z,+z
        for(int_t i=0;i<2*n_dim;++i){
            if(neighbors[i]!= NULL && neighbors[i]->level < level){
                neighbors[i]->divide(nodes, true);
            }
        }

        //Set children's neighbors (first do the easy ones)
        // all of the children live next to each other
        children[0]->set_neighbor(children[1],1);
        children[0]->set_neighbor(children[2],3);
        children[1]->set_neighbor(children[3],3);
        children[2]->set_neighbor(children[3],1);

        if(n_dim==3){
            children[4]->set_neighbor(children[5],1);
            children[4]->set_neighbor(children[6],3);
            children[5]->set_neighbor(children[7],3);
            children[6]->set_neighbor(children[7],1);

            children[0]->set_neighbor(children[4],5);
            children[1]->set_neighbor(children[5],5);
            children[2]->set_neighbor(children[6],5);
            children[3]->set_neighbor(children[7],5);
        }

        //std::cout<<children[0]->neighbors[1]->key<<","<<children[1]->key<<std::endl;
        //std::cout<<children[1]->neighbors[0]->key<<","<<children[0]->key<<std::endl;

        // -x direction
        if(neighbors[0] != NULL && !(neighbors[0]->is_leaf())){
            children[0]->set_neighbor(neighbors[0]->children[1],0);
            children[2]->set_neighbor(neighbors[0]->children[3],0);
        }
        else{
            children[0]->set_neighbor(neighbors[0],0);
            children[2]->set_neighbor(neighbors[0],0);
        }
        // +x direction
        if(neighbors[1] != NULL && !neighbors[1]->is_leaf()){
            children[1]->set_neighbor(neighbors[1]->children[0],1);
            children[3]->set_neighbor(neighbors[1]->children[2],1);
        }else{
            children[1]->set_neighbor(neighbors[1],1);
            children[3]->set_neighbor(neighbors[1],1);
        }
        // -y direction
        if(neighbors[2] != NULL && !neighbors[2]->is_leaf()){
            children[0]->set_neighbor(neighbors[2]->children[2],2);
            children[1]->set_neighbor(neighbors[2]->children[3],2);
        }else{
            children[0]->set_neighbor(neighbors[2],2);
            children[1]->set_neighbor(neighbors[2],2);
        }
        // +y direction
        if(neighbors[3] != NULL && !neighbors[3]->is_leaf()){
            children[2]->set_neighbor(neighbors[3]->children[0],3);
            children[3]->set_neighbor(neighbors[3]->children[1],3);
        }else{
            children[2]->set_neighbor(neighbors[3],3);
            children[3]->set_neighbor(neighbors[3],3);
        }
        if(n_dim==3){
            // -x direction
            if(neighbors[0] != NULL && !(neighbors[0]->is_leaf())){
                children[4]->set_neighbor(neighbors[0]->children[5],0);
                children[6]->set_neighbor(neighbors[0]->children[7],0);
            }
            else{
                children[4]->set_neighbor(neighbors[0],0);
                children[6]->set_neighbor(neighbors[0],0);
            }
            // +x direction
            if(neighbors[1] != NULL && !neighbors[1]->is_leaf()){
                children[5]->set_neighbor(neighbors[1]->children[4],1);
                children[7]->set_neighbor(neighbors[1]->children[6],1);
            }else{
                children[5]->set_neighbor(neighbors[1],1);
                children[7]->set_neighbor(neighbors[1],1);
            }
            // -y direction
            if(neighbors[2] != NULL && !neighbors[2]->is_leaf()){
                children[4]->set_neighbor(neighbors[2]->children[6],2);
                children[5]->set_neighbor(neighbors[2]->children[7],2);
            }else{
                children[4]->set_neighbor(neighbors[2],2);
                children[5]->set_neighbor(neighbors[2],2);
            }
            // +y direction
            if(neighbors[3] != NULL && !neighbors[3]->is_leaf()){
                children[6]->set_neighbor(neighbors[3]->children[4],3);
                children[7]->set_neighbor(neighbors[3]->children[5],3);
            }else{
                children[6]->set_neighbor(neighbors[3],3);
                children[7]->set_neighbor(neighbors[3],3);
            }
            // -z direction
            if(neighbors[4] != NULL && !neighbors[4]->is_leaf()){
                children[0]->set_neighbor(neighbors[4]->children[4],4);
                children[1]->set_neighbor(neighbors[4]->children[5],4);
                children[2]->set_neighbor(neighbors[4]->children[6],4);
                children[3]->set_neighbor(neighbors[4]->children[7],4);
            }else{
                children[0]->set_neighbor(neighbors[4],4);
                children[1]->set_neighbor(neighbors[4],4);
                children[2]->set_neighbor(neighbors[4],4);
                children[3]->set_neighbor(neighbors[4],4);
            }
            // +z direction
            if(neighbors[5] != NULL && !neighbors[5]->is_leaf()){
                children[4]->set_neighbor(neighbors[5]->children[0],5);
                children[5]->set_neighbor(neighbors[5]->children[1],5);
                children[6]->set_neighbor(neighbors[5]->children[2],5);
                children[7]->set_neighbor(neighbors[5]->children[3],5);
            }else{
                children[4]->set_neighbor(neighbors[5],5);
                children[5]->set_neighbor(neighbors[5],5);
                children[6]->set_neighbor(neighbors[5],5);
                children[7]->set_neighbor(neighbors[5],5);
            }
        }

    }
    if(!force){
        for(int_t i=0;i<(1<<n_dim);++i){
            children[i]->divide(nodes);
        }
    }
};

void Cell::build_cell_vector(cell_vec_t& cells){
    if(this->is_leaf()){
        cells.push_back(this);
        return;
    }
    for(int_t i=0;i< 1<<n_dim;++i){
        children[i]->build_cell_vector(cells);
    }
}

bool Cell::inside_triangle(double x, double y, int_t direction){
    int p0x,p0y,p1x,p1y,p2x,p2y;

    p0x = center[0];
    p0y = center[1];
    if(direction==0){
      p1x = neighbors[0]->children[1]->center[0];
      p1y = neighbors[0]->children[1]->center[1];
      p2x = neighbors[0]->children[3]->center[0];
      p2y = neighbors[0]->children[3]->center[1];
    }else if(direction==1){
      p1x = neighbors[1]->children[0]->center[0];
      p1y = neighbors[1]->children[0]->center[1];
      p2x = neighbors[1]->children[2]->center[0];
      p2y = neighbors[1]->children[2]->center[1];
    }else if(direction==2){
      p1x = neighbors[2]->children[2]->center[0];
      p1y = neighbors[2]->children[2]->center[1];
      p2x = neighbors[2]->children[3]->center[0];
      p2y = neighbors[2]->children[3]->center[1];
    }else{
      p1x = neighbors[3]->children[0]->center[0];
      p1y = neighbors[3]->children[0]->center[1];
      p2x = neighbors[3]->children[1]->center[0];
      p2y = neighbors[3]->children[1]->center[1];
    }

    int A = (-p1y*p2x+p0y*(p2x-p1x)+p0x*(p1y-p2y)+p1x*p2y);
    int sign = (A<0)?-1:1;
    double s = (p0y*p2x-p0x*p2y+(p2y-p0y)*x+(p0x-p2x)*y)*sign;
    double t = (p0x*p1y-p0y*p1x+(p0y-p1y)*x+(p1x-p0x)*y)*sign;
    return s>0 && t>0 && (s+t)<A*sign;
}

Cell* Cell::containing_cell(double x, double y, double z){
    if(is_leaf()){
      return this;
    }
    if(z<=center[2]){
        if(y<=center[1]){
          if(x<=center[0]){
            return children[0]->containing_cell(x,y,z);
          }else{
            return children[1]->containing_cell(x,y,z);
          }
        }else{
          if(x<=center[0]){
            return children[2]->containing_cell(x,y,z);
          }else{
            return children[3]->containing_cell(x,y,z);
          }
        }
    }else{
        if(y<center[1]){
          if(x<center[0]){
            return children[4]->containing_cell(x,y,z);
          }else{
            return children[5]->containing_cell(x,y,z);
          }
        }else{
          if(x<center[0]){
            return children[6]->containing_cell(x,y,z);
          }else{
            return children[7]->containing_cell(x,y,z);
          }
        }
    }
};

Cell::~Cell(){
        if(is_leaf()){
            return;
        }
        for(int_t i=0; i< (1<<n_dim);++i){
            delete children[i];
        }
};

Tree::Tree(int_t ndim){
    nx = 0;
    ny = 0;
    nz = 0;
    n_dim = ndim;
    max_level = 0;
    root = NULL;
};

void Tree::set_level(int_t levels){
    max_level = levels;
    nx = 2<<max_level;
    ny = 2<<max_level;
    nz = ((n_dim==3)? 2<<max_level:0);
};

void Tree::build_tree(function test_func){

    Node* points[8];

    points[0] = new Node( 0, 0, 0);
    points[1] = new Node(nx, 0, 0);
    points[2] = new Node( 0,ny, 0);
    points[3] = new Node(nx,ny, 0);
    if(n_dim==3){
        points[4] = new Node( 0, 0,nz);
        points[5] = new Node(nx, 0,nz);
        points[6] = new Node( 0,ny,nz);
        points[7] = new Node(nx,ny,nz);
    }
    for(int_t i=0;i< (1<<n_dim); ++i){
        nodes[points[i]->key] = points[i];
    }
    std::cout<<"Starting to build with dimensions: "<<n_dim<<std::endl;
    root = new Cell(points, n_dim, max_level, test_func);
    root->divide(nodes);
    root->build_cell_vector(cells);
    std::cout<<"Built cell vector of length "<< cells.size()<<std::endl;

    if(n_dim==3){
        //Generate Faces and edges
        for(std::vector<Cell *>::size_type i =0; i!= cells.size(); i++){
            Cell *cell = cells[i];
            Node *p1 = cell->points[0];
            Node *p2 = cell->points[1];
            Node *p3 = cell->points[2];
            Node *p4 = cell->points[3];
            Node *p5 = cell->points[4];
            Node *p6 = cell->points[5];
            Node *p7 = cell->points[6];
            Node *p8 = cell->points[7];

            Edge *ex1, *ex2, *ex3, *ex4;
            Edge *ey1, *ey2, *ey3, *ey4;
            Edge *ez1, *ez2, *ez3, *ez4;

            ex1 = set_default_edge(edges_x, *p1, *p2);
            ex2 = set_default_edge(edges_x, *p3, *p4);
            ex3 = set_default_edge(edges_x, *p5, *p6);
            ex4 = set_default_edge(edges_x, *p7, *p8);

            ey1 = set_default_edge(edges_y, *p1, *p3);
            ey2 = set_default_edge(edges_y, *p2, *p4);
            ey3 = set_default_edge(edges_y, *p5, *p7);
            ey4 = set_default_edge(edges_y, *p6, *p8);

            ez1 = set_default_edge(edges_z, *p1, *p5);
            ez2 = set_default_edge(edges_z, *p2, *p6);
            ez3 = set_default_edge(edges_z, *p3, *p7);
            ez4 = set_default_edge(edges_z, *p4, *p8);

            Face *fx1, *fx2, *fy1, *fy2, *fz1, *fz2;
            fx1 = set_default_face(faces_x, *p1, *p3, *p5, *p7);
            fx2 = set_default_face(faces_x, *p2, *p4, *p6, *p8);
            fy1 = set_default_face(faces_y, *p1, *p2, *p5, *p6);
            fy2 = set_default_face(faces_y, *p3, *p4, *p7, *p8);
            fz1 = set_default_face(faces_z, *p1, *p2, *p3, *p4);
            fz2 = set_default_face(faces_y, *p5, *p6, *p7, *p8);

            fx1->edges[0] = ez1;
            fx1->edges[1] = ey3;
            fx1->edges[2] = ez3;
            fx1->edges[3] = ey1;

            fx2->edges[0] = ez2;
            fx2->edges[1] = ey4;
            fx2->edges[2] = ez4;
            fx2->edges[3] = ey2;

            fy1->edges[0] = ez1;
            fy1->edges[1] = ex3;
            fy1->edges[2] = ez2;
            fy1->edges[3] = ex1;

            fy2->edges[0] = ez3;
            fy2->edges[1] = ex4;
            fy2->edges[2] = ez4;
            fy2->edges[3] = ex2;

            fz1->edges[0] = ey1;
            fz1->edges[1] = ex2;
            fz1->edges[2] = ey2;
            fz1->edges[3] = ex1;

            fz2->edges[0] = ey3;
            fz2->edges[1] = ex4;
            fz2->edges[2] = ey4;
            fz2->edges[3] = ex3;

            fx1->reference++;
            fx2->reference++;
            fy1->reference++;
            fy2->reference++;
            fz1->reference++;
            fz2->reference++;

        }

        //Process hanging x faces
        for(face_it_type it = faces_x.begin(); it!= faces_x.end(); ++it){
            Face *face = it->second;
            if(face->reference<2){
                int_t x;
                x = face->location[0];
                if( x==0 || x==nx) continue; // Face was on the outside, and is not hanging
                if(nodes.count(face->key)) continue; // I will have children (there is a node at my center)
                Node *node;

                //Find Parent
                int_t parent_node;
                for(int_t i=0;i<4;++i){
                    node = face->points[i];
                    if(faces_x.count(node->key)){
                        face->parent = faces_x.at(node->key);
                        parent_node = i;
                        break;
                    }
                }
                //all of my edges are hanging, and most of my points
                for(int_t i=0;i>4;++i){
                    face->edges[i]->hanging=true;
                    face->points[i]->hanging = true;
                }
                // the point oposite the parent node key should not be hanging
                // and also label the edges' parents
                if(parent_node==0){
                    face->points[3]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[2];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[1];

                    face->edges[2]->parents[0] = face->parent->edges[2];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[1];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[1]->parents[0] = face->parent->points[1];
                    face->points[1]->parents[1] = face->parent->points[3];
                    face->points[1]->parents[2] = face->parent->points[1];
                    face->points[1]->parents[3] = face->parent->points[3];

                    face->points[2]->parents[0] = face->parent->points[2];
                    face->points[2]->parents[1] = face->parent->points[3];
                    face->points[2]->parents[2] = face->parent->points[2];
                    face->points[2]->parents[3] = face->parent->points[3];

                }else if(parent_node==1){
                    face->points[2]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[0];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[1];

                    face->edges[2]->parents[0] = face->parent->edges[0];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[1];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[0]->parents[0] = face->parent->points[0];
                    face->points[0]->parents[1] = face->parent->points[2];
                    face->points[0]->parents[2] = face->parent->points[0];
                    face->points[0]->parents[3] = face->parent->points[2];

                    face->points[3]->parents[0] = face->parent->points[2];
                    face->points[3]->parents[1] = face->parent->points[3];
                    face->points[3]->parents[2] = face->parent->points[2];
                    face->points[3]->parents[3] = face->parent->points[3];
                }else if(parent_node==2){
                    face->points[1]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[2];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[3];

                    face->edges[2]->parents[0] = face->parent->edges[2];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[3];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[0]->parents[0] = face->parent->points[0];
                    face->points[0]->parents[1] = face->parent->points[1];
                    face->points[0]->parents[2] = face->parent->points[0];
                    face->points[0]->parents[3] = face->parent->points[1];

                    face->points[3]->parents[0] = face->parent->points[1];
                    face->points[3]->parents[1] = face->parent->points[3];
                    face->points[3]->parents[2] = face->parent->points[1];
                    face->points[3]->parents[3] = face->parent->points[3];
                }else{
                    face->points[0]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[0];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[3];

                    face->edges[2]->parents[0] = face->parent->edges[0];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[3];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[1]->parents[0] = face->parent->points[0];
                    face->points[1]->parents[1] = face->parent->points[1];
                    face->points[1]->parents[2] = face->parent->points[0];
                    face->points[1]->parents[3] = face->parent->points[1];

                    face->points[2]->parents[0] = face->parent->points[0];
                    face->points[2]->parents[1] = face->parent->points[2];
                    face->points[2]->parents[2] = face->parent->points[0];
                    face->points[2]->parents[3] = face->parent->points[2];
                }

                face->hanging = true;
                hanging_faces_x.push_back(face);
                for(int_t i=0; i<4; ++i)
                    node->parents[i] = face->parent->points[i];
            }
        }

        //Process hanging y faces
        for(face_it_type it = faces_y.begin(); it!= faces_y.end(); ++it){
            Face *face = it->second;
            if(face->reference<2){
                int_t y;
                y = face->location[0];
                if( y==0 || y==ny){
                    // Face was on the outside, and is not hanging
                    continue;
                }
                //check if I am a parent or a child
                if(nodes.count(face->key)){
                    // I will have children (there is a node at my center)
                    continue;
                }
                Node *node;

                //Find Parent
                int_t parent_node;
                for(int_t i=0;i<4;++i){
                    node = face->points[i];
                    if(faces_y.count(node->key)){
                        face->parent = faces_y.at(node->key);
                        parent_node = i;
                        break;
                    }
                }
                //all of my edges are hanging
                for(int_t i=0;i>4;++i){
                    face->edges[i]->hanging=true;
                }

                for(int_t i=0;i<4;++i){
                    face->points[i]->hanging = true;
                }
                //the point oposite the parent node key should not be hanging
                // and also determin parents of edges and nodes
                if(parent_node==0){
                    face->points[3]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[2];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[1];

                    face->edges[2]->parents[0] = face->parent->edges[2];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[1];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[1]->parents[0] = face->parent->points[1];
                    face->points[1]->parents[1] = face->parent->points[3];
                    face->points[1]->parents[2] = face->parent->points[1];
                    face->points[1]->parents[3] = face->parent->points[3];

                    face->points[2]->parents[0] = face->parent->points[2];
                    face->points[2]->parents[1] = face->parent->points[3];
                    face->points[2]->parents[2] = face->parent->points[2];
                    face->points[2]->parents[3] = face->parent->points[3];

                }else if(parent_node==1){
                    face->points[2]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[0];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[1];

                    face->edges[2]->parents[0] = face->parent->edges[0];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[1];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[0]->parents[0] = face->parent->points[0];
                    face->points[0]->parents[1] = face->parent->points[2];
                    face->points[0]->parents[2] = face->parent->points[0];
                    face->points[0]->parents[3] = face->parent->points[2];

                    face->points[3]->parents[0] = face->parent->points[2];
                    face->points[3]->parents[1] = face->parent->points[3];
                    face->points[3]->parents[2] = face->parent->points[2];
                    face->points[3]->parents[3] = face->parent->points[3];
                }else if(parent_node==2){
                    face->points[1]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[2];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[3];

                    face->edges[2]->parents[0] = face->parent->edges[2];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[3];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[0]->parents[0] = face->parent->points[0];
                    face->points[0]->parents[1] = face->parent->points[1];
                    face->points[0]->parents[2] = face->parent->points[0];
                    face->points[0]->parents[3] = face->parent->points[1];

                    face->points[3]->parents[0] = face->parent->points[1];
                    face->points[3]->parents[1] = face->parent->points[3];
                    face->points[3]->parents[2] = face->parent->points[1];
                    face->points[3]->parents[3] = face->parent->points[3];
                }else{
                    face->points[0]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[0];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[3];

                    face->edges[2]->parents[0] = face->parent->edges[0];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[3];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[1]->parents[0] = face->parent->points[0];
                    face->points[1]->parents[1] = face->parent->points[1];
                    face->points[1]->parents[2] = face->parent->points[0];
                    face->points[1]->parents[3] = face->parent->points[1];

                    face->points[2]->parents[0] = face->parent->points[0];
                    face->points[2]->parents[1] = face->parent->points[2];
                    face->points[2]->parents[2] = face->parent->points[0];
                    face->points[2]->parents[3] = face->parent->points[2];
                }

                face->hanging = true;
                hanging_faces_y.push_back(face);
                for(int_t i=0; i<4; ++i){
                    node->parents[i] = face->parent->points[i];
                }
            }
        }

        //Process hanging z faces
        for(face_it_type it = faces_z.begin(); it!= faces_z.end(); ++it){
            Face *face = it->second;
            if(face->reference<2){
                int_t z;
                z = face->location[0];
                if( z==0 || z==nz){
                    // Face was on the outside, and is not hanging
                    continue;
                }
                //check if I am a parent or a child
                if(nodes.count(face->key)){
                    // I will have children (there is a node at my center)
                    continue;
                }
                Node *node;

                //Find Parent
                int_t parent_node;
                for(int_t i=0;i<4;++i){
                    node = face->points[i];
                    if(faces_z.count(node->key)){
                        face->parent = faces_z.at(node->key);
                        parent_node = i;
                        break;
                    }
                }
                //all of my edges are hanging
                for(int_t i=0;i>4;++i){
                    face->edges[i]->hanging=true;
                }

                for(int_t i=0;i<4;++i){
                    face->points[i]->hanging = true;
                }
                //the point oposite the parent node key should not be hanging
                // and also label the edges
                if(parent_node==0){
                    face->points[3]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[2];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[1];

                    face->edges[2]->parents[0] = face->parent->edges[2];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[1];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[1]->parents[0] = face->parent->points[1];
                    face->points[1]->parents[1] = face->parent->points[3];
                    face->points[1]->parents[2] = face->parent->points[1];
                    face->points[1]->parents[3] = face->parent->points[3];

                    face->points[2]->parents[0] = face->parent->points[2];
                    face->points[2]->parents[1] = face->parent->points[3];
                    face->points[2]->parents[2] = face->parent->points[2];
                    face->points[2]->parents[3] = face->parent->points[3];

                }else if(parent_node==1){
                    face->points[2]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[0];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[1];

                    face->edges[2]->parents[0] = face->parent->edges[0];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[1];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[0]->parents[0] = face->parent->points[0];
                    face->points[0]->parents[1] = face->parent->points[2];
                    face->points[0]->parents[2] = face->parent->points[0];
                    face->points[0]->parents[3] = face->parent->points[2];

                    face->points[3]->parents[0] = face->parent->points[2];
                    face->points[3]->parents[1] = face->parent->points[3];
                    face->points[3]->parents[2] = face->parent->points[2];
                    face->points[3]->parents[3] = face->parent->points[3];
                }else if(parent_node==2){
                    face->points[1]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[2];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[3];

                    face->edges[2]->parents[0] = face->parent->edges[2];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[3];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[0]->parents[0] = face->parent->points[0];
                    face->points[0]->parents[1] = face->parent->points[1];
                    face->points[0]->parents[2] = face->parent->points[0];
                    face->points[0]->parents[3] = face->parent->points[1];

                    face->points[3]->parents[0] = face->parent->points[1];
                    face->points[3]->parents[1] = face->parent->points[3];
                    face->points[3]->parents[2] = face->parent->points[1];
                    face->points[3]->parents[3] = face->parent->points[3];
                }else{
                    face->points[0]->hanging = false;

                    face->edges[0]->parents[0] = face->parent->edges[0];
                    face->edges[0]->parents[1] = face->parent->edges[0];

                    face->edges[1]->parents[0] = face->parent->edges[1];
                    face->edges[1]->parents[1] = face->parent->edges[3];

                    face->edges[2]->parents[0] = face->parent->edges[0];
                    face->edges[2]->parents[1] = face->parent->edges[2];

                    face->edges[3]->parents[0] = face->parent->edges[3];
                    face->edges[3]->parents[1] = face->parent->edges[3];

                    face->points[1]->parents[0] = face->parent->points[0];
                    face->points[1]->parents[1] = face->parent->points[1];
                    face->points[1]->parents[2] = face->parent->points[0];
                    face->points[1]->parents[3] = face->parent->points[1];

                    face->points[2]->parents[0] = face->parent->points[0];
                    face->points[2]->parents[1] = face->parent->points[2];
                    face->points[2]->parents[2] = face->parent->points[0];
                    face->points[2]->parents[3] = face->parent->points[2];
                }

                face->hanging = true;
                hanging_faces_z.push_back(face);
                for(int_t i=0; i<4; ++i){
                    node->parents[i] = face->parent->points[i];
                }
            }
        }
    }
    else{

        std::cout<<"Building Edges and 1 face"<<std::endl;
        //Generate Edges (and 1 face for consistency)
        for(std::vector<Cell *>::size_type i=0; i!= cells.size(); i++){
            Cell *cell = cells[i];
            Node *p[4];
            for(int_t i=0;i<4;++i)
                p[i] = cell->points[i];
            Edge *e[4];
            e[0] = set_default_edge(edges_y, *p[0], *p[2]);
            e[1] = set_default_edge(edges_x, *p[2], *p[3]);
            e[2] = set_default_edge(edges_y, *p[1], *p[3]);
            e[3] = set_default_edge(edges_x, *p[0], *p[1]);

            Face *face = set_default_face(faces_z, *p[0], *p[1], *p[2], *p[3]);
            for(int_t i=0;i<4;++i){
                cell->edges[i] = e[i];
                face->edges[i] = e[i];
                e[i]->reference++;
            }

            face->hanging=false;
        }
        std::cout<<"Finshished with "<<faces_z.size()<<" faces and "<<edges_x.size()+edges_y.size()<< "edges"<<std::endl;

        //Process hanging x edges
        for(edge_it_type it = edges_x.begin(); it != edges_x.end(); ++it){
            Edge *edge = it->second;
            if(edge->reference <2){
                int_t y = edge->location[1];
                if(y==0 or y==ny) continue; //I am on the boundary
                if(nodes.count(edge->key)) continue; //I am a parent
                //I am a hanging edge find my parent
                Node *node;
                if(edges_x.count(edge->points[0]->key)){
                    node = edge->points[0];
                }else{
                    node = edge->points[1];
                }
                edge->parents[0] = edges_x.at(node->key);
                edge->parents[1] = edge->parents[0];

                node->hanging = true;
                for(int_t i=0;i<4;++i)
                    node->parents[i] = edge->parents[0]->points[i%2];
                edge->hanging = true;
            }
        }

        //Process hanging y edges
        for(edge_it_type it = edges_y.begin(); it != edges_y.end(); ++it){
            Edge *edge = it->second;
            if(edge->reference <2){
                int_t x = edge->location[0];
                if(x==0 or x==nx) continue; //I am on the boundary
                if(nodes.count(edge->key)) continue; //I am a parent
                //I am a hanging edge find my parent
                Node *node;
                if(edges_y.count(edge->points[0]->key)){
                    node = edge->points[0];
                }else{
                    node = edge->points[1];
                }
                edge->parents[0] = edges_y.at(node->key);
                edge->parents[1] = edge->parents[0];

                node->hanging = true;
                for(int_t i=0;i<4;++i)
                    node->parents[i] = edge->parents[0]->points[i%2];
                edge->hanging = true;
            }
        }
    }


    //List hanging edges x
    for(edge_it_type it = edges_x.begin(); it != edges_x.end(); ++it){
        Edge *edge = it->second;
        if(edge->hanging){
            hanging_edges_x.push_back(edge);
        }
    }
    //List hanging edges y
    for(edge_it_type it = edges_x.begin(); it != edges_x.end(); ++it){
        Edge *edge = it->second;
        if(edge->hanging){
            hanging_edges_y.push_back(edge);
        }
    }
    if(n_dim==3){
        //List hanging edges z
        for(edge_it_type it = edges_x.begin(); it != edges_x.end(); ++it){
            Edge *edge = it->second;
            if(edge->hanging){
                hanging_edges_z.push_back(edge);
            }
        }
    }

    //List hanging nodes
    for(node_it_type it = nodes.begin(); it != nodes.end(); ++it){
        Node *node = it->second;
        if(node->hanging){
            hanging_nodes.push_back(node);
        }
    }
};

void Tree::number(){
    //Number Nodes
    int_t ii, ih;
    ii = 0;
    ih = nodes.size()-hanging_nodes.size();
    for(node_it_type it = nodes.begin(); it != nodes.end(); ++it){
        Node *node = it->second;
        if(node->hanging){
            node->index = ih;
            ++ih;
        }else{
            node->index = ii;
            ++ii;
        }
    }

    //Number Cells
    for(std::vector<Cell *>::size_type i =0; i!= cells.size(); ++i)
        cells[i]->index=i;

    //Number edges_x
    ii = 0;
    ih = edges_x.size()-hanging_edges_x.size();
    for(edge_it_type it = edges_x.begin(); it != edges_x.end(); ++it){
        Edge *edge = it->second;
        if(edge->hanging){
          edge->index = ih;
          ++ih;
        }else{
          edge->index = ii;
          ++ii;
        }
    }
    //Number edges_y
    ii = 0;
    ih = edges_y.size()-hanging_edges_y.size();
    for(edge_it_type it = edges_y.begin(); it != edges_y.end(); ++it){
        Edge *edge = it->second;
        if(edge->hanging){
          edge->index = ih;
          ++ih;
        }else{
          edge->index = ii;
          ++ii;
        }
    }

    //Number faces_z
    ii = 0;
    ih = faces_z.size()-hanging_faces_z.size();
    for(face_it_type it = faces_z.begin(); it != faces_z.end(); ++it){
        Face *face = it->second;
        if(face->hanging){
            face->index = ih;
            ++ih;
        }else{
            face->index = ii;
            ++ii;
        }
    }

    if(n_dim==3){
        //Number faces_x
        ii = 0;
        ih = faces_x.size()-hanging_faces_x.size();
        for(face_it_type it = faces_x.begin(); it != faces_x.end(); ++it){
            Face *face = it->second;
            if(face->hanging){
                face->index = ih;
                ++ih;
            }else{
                face->index = ii;
                ++ii;
            }
        }
        //Number faces_y
        ii = 0;
        ih = faces_y.size()-hanging_faces_y.size();
        for(face_it_type it = faces_y.begin(); it != faces_y.end(); ++it){
            Face *face = it->second;
            if(face->hanging){
                face->index = ih;
                ++ih;
            }else{
                face->index = ii;
                ++ii;
            }
        }
        //Number edges_z
        ii = 0;
        ih = edges_z.size()-hanging_edges_z.size();
        for(edge_it_type it = edges_z.begin(); it != edges_z.end(); ++it){
            Edge *edge = it->second;
            if(edge->hanging){
              edge->index = ih;
              ++ih;
            }else{
              edge->index = ii;
              ++ii;
            }
        }
    }

};

Tree::~Tree(){
    if (root==NULL){
        return;
    }
    delete root;
    for(node_it_type it = nodes.begin(); it != nodes.end(); it++){
        delete it->second;
    }
    for(face_it_type it = faces_x.begin(); it != faces_x.end(); it++){
        delete it->second;
    }
    for(face_it_type it = faces_y.begin(); it != faces_y.end(); it++){
        delete it->second;
    }
    for(face_it_type it = faces_z.begin(); it != faces_z.end(); it++){
        delete it->second;
    }
    for(edge_it_type it = edges_x.begin(); it != edges_x.end(); it++){
        delete it->second;
    }
    for(edge_it_type it = edges_y.begin(); it != edges_y.end(); it++){
        delete it->second;
    }
    for(edge_it_type it = edges_z.begin(); it != edges_z.end(); it++){
        delete it->second;
    }
    cells.clear();
    nodes.clear();
    faces_x.clear();
    faces_y.clear();
    faces_z.clear();
    edges_x.clear();
    edges_y.clear();
    edges_z.clear();
};

Cell* Tree::containing_cell(double x, double y, double z){
    return root->containing_cell(x,y,z);
}
