#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h> 

#define MO_EDIT_PROCESS_LINE_SWIGGLE 0.02

#define FALSE 0
#define TRUE !FALSE


typedef struct{
	float *vertex_array;
	unsigned int vertex_count;
	unsigned int ref_count;
	unsigned int *ref;
	unsigned int *material;
}ProcessOutput;

typedef enum{
	MO_LST_GROUND,
	MO_LST_HOLE,
	MO_LST_ROCK,
	MO_LST_BUILDING,
	MO_LST_INDOORS,
	MO_LST_FACTORY_UNASIGNED,
	MO_LST_COUNT
}MOLevelSurfaceType;

typedef struct{
	unsigned int ref[2];
	unsigned int type[2];
	unsigned int next[2];
	unsigned int prev[2];
}MoEditEdge;

typedef struct{
	unsigned int *loop;
	unsigned int loop_count;
	unsigned int material;
	unsigned int start;
	int used;
	unsigned int generation;
	int direction;
}MoEditLoop;

typedef struct{ 
	MoEditEdge *edges;
	unsigned int edges_allocated;
	unsigned int edge_count;
	float *vertex_array;
	unsigned int *vertex_users;
	unsigned int vertex_allocated;
	unsigned int vertex_count;
	MoEditLoop *loops;
	unsigned int loop_count;
	unsigned int loop_alloc;
	unsigned int ref_count;
	unsigned int *ref;
	unsigned int *material;
}MoEditProcess;

#define MO_EDIT_PROCESS_COLAPSE_SIZE (1.0 / 5120.0)

extern void mo_edit_process_edge_end_extracate(MoEditProcess *edit, unsigned int edge, unsigned int end);
extern void mo_edit_process_edge_set(MoEditProcess *edit, unsigned int ref_a, unsigned int ref_b, unsigned int material_a, unsigned int material_b);

#define f_sqrt_step(shift) \
    if((0x40000000l >> shift) + root <= value)          \
    {                                                   \
        value -= (0x40000000l >> shift) + root;         \
        root = (root >> 1) | (0x40000000l >> shift);    \
    }                                                   \
    else                                                \
    {                                                   \
        root = root >> 1;                               \
    }

long f_sqrti(long value)
{
    long root = 0;
    f_sqrt_step(0);
    f_sqrt_step(2);
    f_sqrt_step(4);
    f_sqrt_step(6);
    f_sqrt_step(8);
    f_sqrt_step(10);
    f_sqrt_step(12);
    f_sqrt_step(14);
    f_sqrt_step(16);
    f_sqrt_step(18);
    f_sqrt_step(20);
    f_sqrt_step(22);
    f_sqrt_step(24);
    f_sqrt_step(26);
    f_sqrt_step(28);
    f_sqrt_step(30);
    if(root < value)
        ++root;
    return root;
}

unsigned long long f_sqrti64(unsigned long long value)
{	
	unsigned long long i, root = 0, add;
	for(i = 32; i > 0;)
	{
		i--;
		add = (((unsigned long long)1) << i) + root;
		if(add * add <= value)
			root = add;
	}
	return root;
}

unsigned char f_normalize_2di(int *point, int fixed_point_multiplyer)
{
	int length;
	length = f_sqrti(point[0] * point[0] + point[1] * point[1]);
	if(length != 0)
	{
		point[0] = (point[0] * fixed_point_multiplyer) / length;
		point[1] = (point[1] * fixed_point_multiplyer) / length;
		return 1;
	}
	return 0;
}

float f_normalize2f(float *vec)
{
	float f;
	f = sqrtf(vec[0] * vec[0] + vec[1] * vec[1]);
	vec[0] /= f;
	vec[1] /= f;
	return f;
}

void mo_edit_process_make_sure(MoEditProcess *edit)
{
	unsigned int i, error;
	for(i = 0; i < edit->edge_count; i++)
	{
		if(edit->edges[i].next[0] >= edit->edge_count * 2 ||
			edit->edges[i].next[1] >= edit->edge_count * 2 ||
			edit->edges[i].prev[0] >= edit->edge_count * 2 ||
			edit->edges[i].prev[1] >= edit->edge_count * 2)
			error = 0;
		if(edit->edges[i].ref[0] >= edit->vertex_count ||
			edit->edges[i].ref[1] >= edit->vertex_count)
			error = 0;
	}
}



int mo_edit_process_inside_test(unsigned int test_ref, float *pos, unsigned int *loop, unsigned int size, float *vertex)
{
	unsigned int i, count = 0;
	float *b, *b2;
	for(i = 0; i < size; i++)
	{
		if(loop[i] == test_ref)
			return FALSE;
		b = &vertex[loop[i] * 2];
		b2 = &vertex[loop[(i + 1) % size] * 2];
		if(b[0] > pos[0] != b2[0] > pos[0])
			if(((b[1] - b2[1]) * (pos[0] - b2[0]) - (b[0] - b2[0]) * (pos[1] - b2[1]) > 0) !=
				((b[1] - b2[1]) * (pos[0] - b2[0]) - (b[0] - b2[0]) * (1000.0 - b2[1]) > 0))
				count++;
	}
	return count % 2 == 1;
}

unsigned int mo_edit_process_material_dominat(unsigned int mat_a, unsigned int mat_b)
{
	unsigned int tmp;
	if(mat_a == -1)
		return mat_b;
	if(mat_b == -1)
		return mat_a;

	
	if(mat_b == MO_LST_HOLE)
		return mat_a;
	if(mat_a == MO_LST_HOLE)
		return mat_b;
	if(mat_a == -3 || mat_b == -3)
		return -3;

/*	if(mat_a == MO_LST_GROUND && mat_b == MO_LST_INDOORS)
		return MO_LST_GROUND;
	if(mat_b == MO_LST_GROUND && mat_a == MO_LST_INDOORS)
		return MO_LST_GROUND;*/
	if(mat_a > mat_b)
		return mat_a;
	return mat_b;
}
unsigned int mo_edit_process_vertex_allocate(MoEditProcess *edit, float x, float y)
{
	float vec[2], f;
	unsigned int i;
	for(i = 0; i < edit->vertex_count; i++)
	{
		vec[0] = edit->vertex_array[i * 2] - x;
		vec[1] = edit->vertex_array[i * 2 + 1] - y;
		f = vec[0] * vec[0] + vec[1] * vec[1];
		if(f < MO_EDIT_PROCESS_COLAPSE_SIZE * MO_EDIT_PROCESS_COLAPSE_SIZE)
			return i;
	}
	if(edit->vertex_allocated == edit->vertex_count)
	{
		edit->vertex_allocated *= 2;
		edit->vertex_array = realloc(edit->vertex_array, (sizeof *edit->vertex_array) * edit->vertex_allocated * 4);
		edit->vertex_users = realloc(edit->vertex_users, (sizeof *edit->vertex_users) * edit->vertex_allocated * 2);
	}
	edit->vertex_array[edit->vertex_count * 2 + 0] = x;
	edit->vertex_array[edit->vertex_count * 2 + 1] = y;
	edit->vertex_users[edit->vertex_count] = -1;
	edit->vertex_count++;
	return edit->vertex_count - 1;
}

unsigned int mo_edit_find_loop(MoEditProcess *edit, unsigned int start, unsigned int *buffer)
{
	unsigned int pos, next, count = 0, last = -1;
	pos = start;
	while(TRUE)
	{
		if(edit->edges[pos / 2].type[0] == edit->edges[pos / 2].type[1] ||
			edit->edges[pos / 2].type[pos % 2] == -2)
			return -1;
		next = edit->edges[pos / 2].next[pos % 2];
		edit->edges[pos / 2].type[pos % 2] = -2;
		if(next / 2 == pos / 2)
			return -1;
		if(last != edit->edges[pos / 2].ref[pos % 2])
			buffer[count++] = last = edit->edges[pos / 2].ref[pos % 2];
		if(next == start)
		{
			if(buffer[0] == buffer[count - 1])
				return count - 1;
			else
				return count;
		}
		pos = next;
	}
}


void mo_edit_process_edge_end_integrate(MoEditProcess *edit, unsigned int edge, unsigned int end)
{
	float center[2], vec[2], last_vec[2], test[2];
	unsigned int vertex, ref, pos, start, prevoius, i = 0;
	vertex = edit->edges[edge].ref[end];
	start = pos = edit->vertex_users[vertex];
	if(pos == -1) /* First to connect */
	{
		edit->edges[edge].next[end] = edge * 2 + (end + 1) % 2;
		edit->edges[edge].prev[(end + 1) % 2] = edge * 2 + end;
		edit->vertex_users[vertex] = edge * 2 + end;
		return;
	}

	if(edit->vertex_users[vertex] / 2 != edge &&
		edit->edges[pos / 2].next[pos % 2] / 2 == pos / 2) /* Second to connect */
	{
		edit->edges[pos / 2].next[pos % 2] = edge * 2 + (end + 1) % 2;
		edit->edges[pos / 2].prev[(pos + 1) % 2] = edge * 2 + end;
		
		edit->edges[edge].next[end] = (pos / 2) * 2 + (pos + 1) % 2;
		edit->edges[edge].prev[(end + 1) % 2] = pos;
		return;
	}

	center[0] = edit->vertex_array[vertex * 2 + 0];
	center[1] = edit->vertex_array[vertex * 2 + 1];

	ref = edit->edges[edge].ref[(end + 1) % 2];
	test[0] = edit->vertex_array[ref * 2 + 0] - center[0];
	test[1] = edit->vertex_array[ref * 2 + 1] - center[1];

	ref = edit->edges[pos / 2].ref[(pos + 1) % 2];
	last_vec[0] = edit->vertex_array[ref * 2 + 0] - center[0];
	last_vec[1] = edit->vertex_array[ref * 2 + 1] - center[1];
	while(TRUE)
	{
		prevoius = pos;
		pos = edit->edges[pos / 2].next[pos % 2];
		if(pos == -1)
			printf("Error");
		ref = edit->edges[pos / 2].ref[pos % 2];

		vec[0] = edit->vertex_array[ref * 2 + 0] - center[0];
		vec[1] = edit->vertex_array[ref * 2 + 1] - center[1];
		if(vec[0] * last_vec[1] - vec[1] * last_vec[0] <= 0)
		{
			if(test[0] * vec[1] - test[1] * vec[0] < 0 ||
				test[0] * last_vec[1] - test[1] * last_vec[0] >= 0)
			{
				edit->edges[edge].next[end % 2] = pos;
				edit->edges[edge].prev[(end + 1) % 2] = edit->edges[pos / 2].prev[pos % 2];
				edit->edges[pos / 2].prev[pos % 2] = edge * 2 + end % 2;
				pos = edit->edges[edge].prev[(end + 1) % 2];
				edit->edges[pos / 2].next[pos % 2] = edge * 2 + (end + 1) % 2;
				return;
			}
		}else
		{
			if(test[0] * vec[1] - test[1] * vec[0] < 0 &&
				test[0] * last_vec[1] - test[1] * last_vec[0] > 0)
			{
				edit->edges[edge].next[end % 2] = pos;
				edit->edges[edge].prev[(end + 1) % 2] = edit->edges[pos / 2].prev[pos % 2];
				edit->edges[pos / 2].prev[pos % 2] = edge * 2 + end % 2;
				pos = edit->edges[edge].prev[(end + 1) % 2];
				edit->edges[pos / 2].next[pos % 2] = edge * 2 + (end + 1) % 2;
				return;
			}
		}
		pos = (pos / 2) * 2 + (pos + 1) % 2;
		if(pos == start)
			i++;
		last_vec[0] = vec[0];
		last_vec[1] = vec[1];
		i++;
	}

}


/*

uint mo_edit_process_vertex_dominance(MoEditProcess *edit)
{
	uint pos, start, next, i, dom;

	for(i = 0; i < edit->edge_count; i++)
	{
		edit->edges[i].type2[0] = edit->edges[i].type[0];
		edit->edges[i].type2[1] = edit->edges[i].type[1];
	}
	for(i = 0; i < edit->vertex_count; i++)
	{
		pos = start = edit->vertex_users[i];
		if(pos != -1)
		{
			while(TRUE)
			{
				next = edit->edges[pos / 2].next[pos % 2];
				dom = mo_edit_process_material_dominat(edit->edges[pos / 2].type[pos % 2], 
														edit->edges[next / 2].type[next % 2]);
				edit->edges[pos / 2].type2[pos % 2] = mo_edit_process_material_dominat(edit->edges[pos / 2].type2[pos % 2], dom);
				edit->edges[next / 2].type2[next % 2] = mo_edit_process_material_dominat(edit->edges[next / 2].type2[next % 2], dom);
				next = (next / 2) * 2 + (next + 1) % 2;
				pos = next;
				if(pos == start)
					break;
			}
		}
	}
	for(i = 0; i < edit->edge_count; i++)
	{
		edit->edges[i].type[0] = edit->edges[i].type2[0];
		edit->edges[i].type[1] = edit->edges[i].type2[1];
	}
	for(i = 0; i < edit->edge_count; i++)
	{
		if(edit->edges[i].type[0] == edit->edges[i].type[1])
		{
			pos = i * 2;
			while(TRUE)
			{
				next = edit->edges[pos / 2].next[pos % 2];
				if(next / 2 == pos / 2)
					break;
				if(next / 2 != edit->edges[pos / 2].prev[(pos + 1) % 2] / 2)
				{
					mo_edit_process_edge_end_extracate(edit, pos / 2, pos % 2);
					break;
				}
				edit->edges[next / 2].type[0] = edit->edges[next / 2].type[1];
				mo_edit_process_edge_end_extracate(edit, pos / 2, pos % 2);
				pos = next;
			}
			pos = i * 2 + 1;
			while(TRUE)
			{
				next = edit->edges[pos / 2].next[pos % 2];
				if(next / 2 == pos / 2)
					break;
				if(next / 2 != edit->edges[pos / 2].prev[(pos + 1) % 2] / 2)
				{
					mo_edit_process_edge_end_extracate(edit, pos / 2, pos % 2);
					break;
				}
				edit->edges[next / 2].type[0] = edit->edges[next / 2].type[1];
				mo_edit_process_edge_end_extracate(edit, pos / 2, pos % 2);
				pos = next;
			}
			mo_edit_process_edge_end_extracate(edit, i, 0);
			mo_edit_process_edge_end_extracate(edit, i, 1);
		}
	}
}
*/
void mo_edit_process_vertex_dominance2(MoEditProcess *edit)
{
	unsigned int i, j, dom;
	float point[4], vec[2], f;
	for(i = 0; i < edit->edge_count; i++)
	{
		vec[0] = edit->vertex_array[edit->edges[i].ref[0] * 2] - edit->vertex_array[edit->edges[i].ref[1] * 2];
		vec[1] = edit->vertex_array[edit->edges[i].ref[0] * 2 + 1] - edit->vertex_array[edit->edges[i].ref[1] * 2 + 1];
		f = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
		point[0] = vec[0] * 0.5 + edit->vertex_array[edit->edges[i].ref[1] * 2] - MO_EDIT_PROCESS_COLAPSE_SIZE * 0.5 * vec[1] / f;
		point[1] = vec[1] * 0.5 + edit->vertex_array[edit->edges[i].ref[1] * 2 + 1] + MO_EDIT_PROCESS_COLAPSE_SIZE * 0.5 * vec[0] / f;
		point[2] = vec[0] * 0.5 + edit->vertex_array[edit->edges[i].ref[1] * 2] + MO_EDIT_PROCESS_COLAPSE_SIZE * 0.5 * vec[1] / f;
		point[3] = vec[1] * 0.5 + edit->vertex_array[edit->edges[i].ref[1] * 2 + 1] - MO_EDIT_PROCESS_COLAPSE_SIZE * 0.5 * vec[0] / f;

		for(j = 0; j < edit->loop_count && edit->edges[i].type[0] != edit->edges[i].type[1]; j++)
		{
			dom = mo_edit_process_material_dominat(edit->edges[i].type[0], edit->loops[j].material);
			if(dom != edit->edges[i].type[0])
				if(mo_edit_process_inside_test(-1, point, edit->loops[j].loop, edit->loops[j].loop_count, edit->vertex_array))
					edit->edges[i].type[0] = dom;
			dom = mo_edit_process_material_dominat(edit->edges[i].type[1], edit->loops[j].material);
			if(dom != edit->edges[i].type[1])
				if(mo_edit_process_inside_test(-1, &point[2], edit->loops[j].loop, edit->loops[j].loop_count, edit->vertex_array))
					edit->edges[i].type[1] = dom;
		}
		if(edit->edges[i].type[0] == edit->edges[i].type[1])
		{
			mo_edit_process_edge_end_extracate(edit, i, 0);
			mo_edit_process_edge_end_extracate(edit, i, 1);
		}
	}
	/*Special case the outer box */
/*	for(i = 0; i < edit->edge_count; i++)
	{
		if(edit->edges[i].type[0] == -1)
			edit->edges[i].type[0] = MO_LST_HOLE;
		else if(edit->edges[i].type[0] == -3)
			edit->edges[i].type[0] = -1;
		if(edit->edges[i].type[1] == -1)
			edit->edges[i].type[1] = MO_LST_HOLE;
		else if(edit->edges[i].type[1] == -3)
			edit->edges[i].type[1] = -1;
	}*/
}


void mo_edit_process_reduce(MoEditProcess *edit)
{
	unsigned int i, j, edge, ref[3], next;
	float point[4], vec[4], f;
	for(i = 0; i < edit->vertex_count; i++)
	{
		edge = edit->vertex_users[i];
		if(edge != -1 && edge / 2 != edit->edges[edge / 2].next[edge % 2] / 2 && edit->edges[edge / 2].prev[(edge + 1) % 2] / 2 == edit->edges[edge / 2].next[edge % 2] / 2)
		{
			ref[0] = edit->edges[edge / 2].ref[(edge + 1) % 2];
			ref[1] = edit->edges[edge / 2].ref[edge % 2];
			next = edit->edges[edge / 2].next[edge % 2];
			ref[2] = edit->edges[next / 2].ref[next % 2];
			vec[0] = edit->vertex_array[ref[0] * 2 + 0] - edit->vertex_array[ref[2] * 2 + 0];
			vec[1] = edit->vertex_array[ref[0] * 2 + 1] - edit->vertex_array[ref[2] * 2 + 1];
			vec[2] = edit->vertex_array[ref[1] * 2 + 0] - edit->vertex_array[ref[2] * 2 + 0];
			vec[3] = edit->vertex_array[ref[1] * 2 + 1] - edit->vertex_array[ref[2] * 2 + 1];
			f_normalize2f(vec);
			f = vec[2] * vec[1] - vec[3] * vec[0];
			if(f < MO_EDIT_PROCESS_COLAPSE_SIZE && f > -MO_EDIT_PROCESS_COLAPSE_SIZE)
			{
				mo_edit_process_edge_end_extracate(edit, edge / 2, edge % 2);
				edit->edges[edge / 2].ref[edge % 2] = ref[2];
				mo_edit_process_edge_end_extracate(edit, next / 2, 0);
				mo_edit_process_edge_end_extracate(edit, next / 2, 1);
				mo_edit_process_edge_end_integrate(edit, edge / 2, edge % 2);
			}
		}
/*		if(edit->edges[i].type[0] == edit->edges[i].type[1])
		{
			mo_edit_process_edge_end_extracate(edit, i, 0);
			mo_edit_process_edge_end_extracate(edit, i, 1);
		}*/
	}
}

/*
When a intersection is found:
Detach user of existing and attach to new vertex:
Add second edge bewean other and and new vertex.
Recursively create 2 new edges form the new edge.
Run Material dominance check on new vertex.

Clean up:
Remove all non conecting ends and all edges with identical sides.

Polygonize:
Extract all lops including outer loops.
see if outer looks fit in to each look.
Merge these loops.
Polygonize.

Be happy!!!

*/

void mo_edit_process_edge_end_extracate(MoEditProcess *edit, unsigned int edge, unsigned int end)
{
	unsigned int vertex, pos;
	vertex = edit->edges[edge].ref[end];
	pos = edit->vertex_users[vertex];

	if(edit->edges[edge].next[end] == edge * 2 + (end + 1) % 2 && edit->vertex_users[vertex] / 2 == edge)
	{
		edit->vertex_users[vertex] = -1;
		return;
	}

	pos = edit->edges[edge].next[end];
	edit->edges[pos / 2].prev[pos % 2] = edit->edges[edge].prev[(end + 1) % 2];
	pos = edit->edges[edge].prev[(end + 1) % 2];
	if(edit->vertex_users[vertex] / 2 == edge)
		edit->vertex_users[vertex] = pos;
	edit->edges[pos / 2].next[pos % 2] = edit->edges[edge].next[end];
	
	edit->edges[edge].next[end] = edge * 2 + (end + 1) % 2;
	edit->edges[edge].prev[(end + 1) % 2] = edge * 2 + end;
}

int mo_edit_process_split_edge(MoEditProcess *edit, unsigned int ref_a, unsigned int ref_b, unsigned int material_a, unsigned int material_b)
{
	float *point_a, *point_b, vector[2], *a, *b, v[2], draw[2];
	unsigned int i, edge_count, ref[2];
	edge_count = edit->edge_count;
	point_a = &edit->vertex_array[ref_a * 2];
	point_b = &edit->vertex_array[ref_b * 2];
	vector[0] = point_b[0] - point_a[0];
	vector[1] = point_b[1] - point_a[1];
	f_normalize2f(vector);
	for(i = 0; i < edge_count; i++)
	{
		a = &edit->vertex_array[edit->edges[i].ref[0] * 2];
		b = &edit->vertex_array[edit->edges[i].ref[1] * 2];
		if((a[0] - point_a[0]) * vector[1] - (a[1] - point_a[1]) * vector[0] > MO_EDIT_PROCESS_COLAPSE_SIZE * 1)
		{
			if((b[0] - point_a[0]) * vector[1] - (b[1] - point_a[1]) * vector[0] < MO_EDIT_PROCESS_COLAPSE_SIZE * -1)
			{
				v[0] = b[0] - a[0];
				v[1] = b[1] - a[1];
				f_normalize2f(v);
				if((point_a[0] - a[0]) * v[1] - (point_a[1] - a[1]) * v[0] < -0.0/*MO_EDIT_PROCESS_COLAPSE_SIZE * -0.1*/)
				{
					if((point_b[0] - a[0]) * v[1] - (point_b[1] - a[1]) * v[0] > 0.0/*MO_EDIT_PROCESS_COLAPSE_SIZE * 0.1*/)
					{
						mo_edit_process_make_sure(edit);					
						f_intersect2f(draw, a, b, point_a, point_b);
						ref[0] = edit->edges[i].ref[0];
						ref[1] = mo_edit_process_vertex_allocate(edit, draw[0], draw[1]);
						if(ref[1] != edit->edges[i].ref[0] && ref[1] != edit->edges[i].ref[0])
						{
							mo_edit_process_edge_end_extracate(edit, i, 0);
							edit->edges[i].ref[0] = ref[1];
							mo_edit_process_edge_end_integrate(edit, i, 0);
							mo_edit_process_make_sure(edit);
							mo_edit_process_edge_set(edit, ref[0], ref[1], edit->edges[i].type[0], edit->edges[i].type[1]);
							mo_edit_process_make_sure(edit);
							mo_edit_process_edge_set(edit, ref_a, ref[1], material_a, material_b);
							mo_edit_process_make_sure(edit);
							mo_edit_process_edge_set(edit, ref[1], ref_b, material_a, material_b);
							mo_edit_process_make_sure(edit);
							return TRUE;
						}
					}
				}
			}
		}else if((a[0] - point_a[0]) * vector[1] - (a[1] - point_a[1]) * vector[0] < MO_EDIT_PROCESS_COLAPSE_SIZE * -1)
		{
			if((b[0] - point_a[0]) * vector[1] - (b[1] - point_a[1]) * vector[0] > MO_EDIT_PROCESS_COLAPSE_SIZE * 1)
			{
				v[0] = b[0] - a[0];
				v[1] = b[1] - a[1];
				f_normalize2f(v);
				if((point_a[0] - a[0]) * v[1] - (point_a[1] - a[1]) * v[0] > MO_EDIT_PROCESS_COLAPSE_SIZE * 1)
				{
					if((point_b[0] - a[0]) * v[1] - (point_b[1] - a[1]) * v[0] < MO_EDIT_PROCESS_COLAPSE_SIZE * -1)
					{
						mo_edit_process_make_sure(edit);
						f_intersect2f(draw, a, b, point_a, point_b);
						ref[0] = edit->edges[i].ref[0];
						ref[1] = mo_edit_process_vertex_allocate(edit, draw[0], draw[1]);
						if(ref[1] != edit->edges[i].ref[0] && ref[1] != edit->edges[i].ref[0])
						{
							mo_edit_process_edge_end_extracate(edit, i, 0);
							edit->edges[i].ref[0] = ref[1];
							mo_edit_process_edge_end_integrate(edit, i, 0);
							mo_edit_process_make_sure(edit);
							mo_edit_process_edge_set(edit, ref[0], ref[1], edit->edges[i].type[0], edit->edges[i].type[1]);
							mo_edit_process_make_sure(edit);
							mo_edit_process_edge_set(edit, ref_a, ref[1], material_a, material_b);
							mo_edit_process_make_sure(edit);
							mo_edit_process_edge_set(edit, ref[1], ref_b, material_a, material_b);
							mo_edit_process_make_sure(edit);
							return TRUE;
						}
					}
				}
			}
		}
	}
	return FALSE;
}


void mo_edit_process_edge_set(MoEditProcess *edit, unsigned int ref_a, unsigned int ref_b, unsigned int material_a, unsigned int material_b)
{
	float vec[2], tmp[2], f, length;
	unsigned int i, edge;
	if(ref_a == ref_b)
		return;
	vec[0] = edit->vertex_array[ref_b * 2 + 0] - edit->vertex_array[ref_a * 2 + 0];
	vec[1] = edit->vertex_array[ref_b * 2 + 1] - edit->vertex_array[ref_a * 2 + 1];
	length = sqrt(vec[0] * vec[0] + vec[1] * vec[1]);
	vec[0] /= length;
	vec[1] /= length;
	for(i = 0; i < edit->vertex_count; i++)
	{
		if(i != ref_a && i != ref_b)
		{
			tmp[0] = edit->vertex_array[i * 2 + 0] - edit->vertex_array[ref_a * 2 + 0];
			tmp[1] = edit->vertex_array[i * 2 + 1] - edit->vertex_array[ref_a * 2 + 1];
			f = tmp[0] * vec[1] - tmp[1] * vec[0];
			if(f < MO_EDIT_PROCESS_COLAPSE_SIZE && f > -MO_EDIT_PROCESS_COLAPSE_SIZE)
			{
				f = tmp[0] * vec[0] + tmp[1] * vec[1];
				if(f > MO_EDIT_PROCESS_COLAPSE_SIZE && f < length - MO_EDIT_PROCESS_COLAPSE_SIZE)
				{
					mo_edit_process_edge_set(edit, ref_a, i, material_a, material_b);
					mo_edit_process_edge_set(edit, i, ref_b, material_a, material_b);
					return;
				}
			}
		} 
	}

	if(edit->vertex_users[ref_a] != -1)
	{
		unsigned int start, pos;
		start = pos = edit->vertex_users[ref_a];
		while(TRUE)
		{
			pos = edit->edges[pos / 2].next[pos % 2];
			if(edit->edges[pos / 2].ref[0] == ref_a &&
				edit->edges[pos / 2].ref[1] == ref_b)
			{
				edit->edges[pos / 2].type[0] = mo_edit_process_material_dominat(edit->edges[pos / 2].type[0], material_a);
				edit->edges[pos / 2].type[1] = mo_edit_process_material_dominat(edit->edges[pos / 2].type[1], material_b);
				return;
			}
			if(edit->edges[pos / 2].ref[0] == ref_b &&
				edit->edges[pos / 2].ref[1] == ref_a)
			{
				edit->edges[pos / 2].type[0] = mo_edit_process_material_dominat(edit->edges[pos / 2].type[0], material_b);
				edit->edges[pos / 2].type[1] = mo_edit_process_material_dominat(edit->edges[pos / 2].type[1], material_a);
				return;
			}
			pos = (pos / 2) * 2 + (pos + 1) % 2;
			if(pos == start)
				break;
		}
	}
	if(mo_edit_process_split_edge(edit, ref_a, ref_b, material_a, material_b))
		return;
	// EDGE test

	if(edit->edges_allocated == edit->edge_count)
	{
		edit->edges_allocated *= 2;
		edit->edges = realloc(edit->edges, (sizeof *edit->edges) * edit->edges_allocated);
	}
	edge = edit->edge_count++;
	edit->edges[edge].type[0] = material_a;
	edit->edges[edge].type[1] = material_b;
	edit->edges[edge].ref[0] = ref_a;
	edit->edges[edge].ref[1] = ref_b;

	edit->edges[edge].next[0] = -1;
	edit->edges[edge].prev[0] = -1;
	edit->edges[edge].next[1] = -1;
	edit->edges[edge].prev[1] = -1;

/*	if(edit->vertex_users[ref_a] == -1)
	{
		edit->edges[edge].next[0] = edge * 2 + 1;
		edit->edges[edge].prev[1] = edge * 2;
		edit->vertex_users[ref_a] = edge * 2;
	}else*/
		mo_edit_process_edge_end_integrate(edit, edge, 0);
/*	if(edit->vertex_users[ref_b] == -1)
	{
		edit->edges[edge].prev[0] = edge * 2 + 1;
		edit->edges[edge].next[1] = edge * 2;
		edit->vertex_users[ref_b] = edge * 2 + 1;
	}else*/
		mo_edit_process_edge_end_integrate(edit, edge, 1);

	if(edit->edges[edge].next[0] == -1 ||
	edit->edges[edge].prev[0] == -1 ||
	edit->edges[edge].next[1] == -1 ||
	edit->edges[edge].prev[1] == -1)
	{
		printf("ERROR");
	}

}


MoEditProcess *mo_edit_process_init(float *vertex_array, unsigned int *loop_sizes, unsigned int loop_count)
{
	unsigned int mirror_count[] = {1, 2, 3, 4, 2, 4};
	MoEditProcess *edit;
	unsigned int i, j, k, size;
	size = loop_sizes[loop_count - 1];
	edit = malloc(sizeof *edit);
	edit->edges_allocated = size * 2 * 256;
	edit->edges = malloc((sizeof *edit->edges) * edit->edges_allocated);
	edit->edge_count = 0;
	edit->vertex_allocated = size * 2;
	edit->vertex_array = malloc((sizeof *edit->vertex_array) * edit->vertex_allocated * 2);
	edit->vertex_users = malloc((sizeof *edit->vertex_users) * edit->vertex_allocated);
	edit->vertex_count = 0;
	edit->loop_alloc = loop_count;
	edit->loops = malloc((sizeof *edit->loops) * edit->loop_alloc);
	edit->loop_count = loop_count;


	for(i = j = 0; i < loop_count; i++)
	{
		edit->loops[i].loop = malloc((sizeof *edit->loops) * (loop_sizes[i] - j));
		edit->loops[i].direction = 0;
		edit->loops[i].material = 0;
		edit->loops[i].loop_count = 0;
		for(; j < loop_sizes[i]; j++)
			edit->loops[i].loop[edit->loops[i].loop_count++] = mo_edit_process_vertex_allocate(edit, vertex_array[j * 2], vertex_array[j * 2 + 1]);
	}

	
	for(i = 0; i < edit->loop_count; i++)
		for(j = 0; j < edit->loops[i].loop_count; j++)
			mo_edit_process_edge_set(edit, edit->loops[i].loop[j], edit->loops[i].loop[(j + 1) % edit->loops[i].loop_count], edit->loops[i].material, -2);
	return edit;
}

void mo_edit_process_surround(MoEditProcess *edit)
{
	float max[2], min[2];
	unsigned int square[4];
	unsigned int i, j, k, size, *ids;

	for(i = 0; i < edit->edge_count; i++)
	{
		if(edit->edges[i].type[0] == -1)
			edit->edges[i].type[0] = MO_LST_HOLE;
		if(edit->edges[i].type[1] == -1)
			edit->edges[i].type[1] = MO_LST_HOLE;
	}
	max[0] = min[0] = edit->vertex_array[0];
	max[1] = min[1] = edit->vertex_array[1];
	for(i = 1; i < edit->vertex_count; i++)
	{
		if(edit->vertex_array[i * 2 + 0] > max[0])
			max[0] = edit->vertex_array[i * 2 + 0];
		if(edit->vertex_array[i * 2 + 0] < min[0])
			min[0] = edit->vertex_array[i * 2 + 0];
		if(edit->vertex_array[i * 2 + 1] > max[1])
			max[1] = edit->vertex_array[i * 2 + 1];
		if(edit->vertex_array[i * 2 + 1] < min[1])
			min[1] = edit->vertex_array[i * 2 + 1];
	}
	square[0] = mo_edit_process_vertex_allocate(edit, min[0] - 0.1, min[1] - 0.1);
	square[1] = mo_edit_process_vertex_allocate(edit, min[0] - 0.1, max[1] + 0.1);
	square[2] = mo_edit_process_vertex_allocate(edit, max[0] + 0.1, max[1] + 0.1);
	square[3] = mo_edit_process_vertex_allocate(edit, max[0] + 0.1, min[1] - 0.1);
	mo_edit_process_edge_set(edit, square[0], square[1], MO_LST_HOLE, -1);
	mo_edit_process_edge_set(edit, square[1], square[2], MO_LST_HOLE, -1);
	mo_edit_process_edge_set(edit, square[2], square[3], MO_LST_HOLE, -1);
	mo_edit_process_edge_set(edit, square[3], square[0], MO_LST_HOLE, -1);
}


void mo_edit_process_loop_extract(MoEditProcess *edit, unsigned int material_id)
{
	float vec[2], f, test[2];
	unsigned int *buffer, count;
	unsigned int i, j, type, uint_;
	buffer = malloc((sizeof *buffer) * edit->edge_count * 2);
	for(i = 0; i < edit->loop_count; i++)
	{
		free(edit->loops[i].loop);
	}
	edit->loop_count = 0;

	for(i = 0; i < edit->edge_count * 2; i++)
	{
		type = edit->edges[i / 2].type[i % 2];
		if(type != -2)
		{
			count = mo_edit_find_loop(edit, i, buffer);
			if(count != -1)
			{		
				if(edit->loop_count == edit->edges_allocated)
				{
					edit->edges_allocated += 16;
					edit->loops = realloc(edit->loops, (sizeof *edit->loops) * edit->edges_allocated);
				}
				edit->loops[edit->loop_count].loop = buffer;
				edit->loops[edit->loop_count].loop_count = count;
				edit->loops[edit->loop_count].material = type;
				edit->loops[edit->loop_count].used = FALSE;
				edit->loop_count++;
				buffer = malloc((sizeof *buffer) * edit->edge_count);
			}
		}
	}
	free(buffer);
}


void mo_edit_process_re_ingest(MoEditProcess *edit)
{
	unsigned int i, j;
	for(i = 0; i < edit->vertex_count; i++)
		edit->vertex_users[i] = -1;
	for(i = 0; i < edit->loop_count; i++)
	{
		if(edit->loops[i].used)
			for(j = 0; j < edit->loops[i].loop_count; j++)
				mo_edit_process_edge_set(edit, edit->loops[i].loop[(j + 1) % edit->loops[i].loop_count], edit->loops[i].loop[j], edit->loops[i].material, -1);
		free(edit->loops[i].loop);
	}
	edit->loop_count = 0;
}

void mo_edit_process_free(MoEditProcess *edit)
{
	free(edit->edges);
	free(edit->vertex_array);

	free(edit->vertex_users);
	free(edit->loops);

	free(edit);
}

int mo_edit_process_loop_direction(unsigned int *loop, unsigned int loop_length, float *vertex)
{
	unsigned int i;
	float sum;
	loop_length--;
	sum = (vertex[loop[0] * 2] - vertex[loop[loop_length] * 2]) * (vertex[loop[0] * 2 + 1] + vertex[loop[loop_length] * 2 + 1]);
	for(i = 0; i < loop_length; i++)
		sum += (vertex[loop[i + 1] * 2] - vertex[loop[i] * 2]) * (vertex[loop[i + 1] * 2 + 1] + vertex[loop[i] * 2 + 1]);
	return sum > 0;
}


void mo_edit_reloop(MoEditProcess *edit)
{
	unsigned int i, j, k, pos, next, start_edge, edge, count = 0, vertex_id, goal_id, *buffer, buffer_use;
	float goal_vec[2], vec[2], f, best;

	buffer = malloc((sizeof *buffer) * edit->edge_count * 2);
	for(i = 0; i < edit->loop_count; i++)
	{
		buffer_use = 0;
		for(j = 0; j < edit->loops[i].loop_count; j++)
		{
			vertex_id = edit->loops[i].loop[j];
			goal_id = edit->loops[i].loop[(j + 1) % edit->loops[i].loop_count];
			while(vertex_id != goal_id)
			{
				start_edge = edge = edit->vertex_users[vertex_id];
				while(TRUE)
				{
					if(edit->edges[edge / 2].ref[(edge + 1) % 2] == goal_id)
					{
						vertex_id = goal_id;
						break;
					}
					
					edge = edit->edges[edge / 2].next[edge % 2];
					edge = (edge / 2) * 2 + (edge + 1) % 2;
					if(start_edge == edge)
						break;
				}
				if(start_edge == edge)
				{
					goal_vec[0] = edit->vertex_array[goal_id * 2 + 0] - edit->vertex_array[vertex_id * 2 + 0];
					goal_vec[1] = edit->vertex_array[goal_id * 2 + 1] - edit->vertex_array[vertex_id * 2 + 1];
					best = 0;
					while(TRUE)
					{
						vec[0] = edit->vertex_array[edit->edges[edge / 2].ref[(edge + 1) % 2] * 2 + 0] - edit->vertex_array[edit->edges[edge / 2].ref[edge % 2] * 2 + 0]; 
						vec[1] = edit->vertex_array[edit->edges[edge / 2].ref[(edge + 1) % 2] * 2 + 1] - edit->vertex_array[edit->edges[edge / 2].ref[edge % 2] * 2 + 1];
						f_normalize2f(vec);
						f = goal_vec[0] * vec[0] + goal_vec[1] * vec[1];
						if(f > best)
						{
							best = f;
							vertex_id = edit->edges[edge / 2].ref[(edge + 1) % 2];
						}
						edge = edit->edges[edge / 2].next[edge % 2];
						edge = (edge / 2) * 2 + (edge + 1) % 2;
						if(start_edge == edge)
							break;
					}
				}
				buffer[buffer_use++] = vertex_id;
			}
		}
		free(edit->loops[i].loop);
		edit->loops[i].loop = malloc((sizeof *edit->loops[i].loop) * buffer_use);
		for(j = 0; j < buffer_use; j++)
			edit->loops[i].loop[j] = buffer[j];
		edit->loops[i].loop_count = buffer_use;
	}
	free(buffer);
}


void mo_edit_process_sort(MoEditProcess *edit)
{
	unsigned int i, j, generation;

	for(i = 0; i < edit->loop_count; i++)
		edit->loops[i].used = TRUE;

	for(i = 0; i < edit->loop_count; i++)
	{
		if(edit->loops[i].used && !mo_edit_process_loop_direction(edit->loops[i].loop, edit->loops[i].loop_count, edit->vertex_array)) /* if an outside shape */
		{
			generation = 0; 
			for(j = 0; j < edit->loop_count; j++) /* see how many layers we are inside */
				if(edit->loops[i].material == edit->loops[j].material)
					if(mo_edit_process_inside_test(edit->loops[i].loop[0], &edit->vertex_array[edit->loops[i].loop[0] * 2], edit->loops[j].loop, edit->loops[j].loop_count, edit->vertex_array))
						generation++;



			if(generation != 0) /* we are on the inside of something */
			{
				edit->loops[i].used = FALSE;
				for(j = 0; j < edit->loop_count; j++) /* remove anything uinside of this one */
					if(edit->loops[i].material == edit->loops[j].material)
						if(mo_edit_process_inside_test(edit->loops[j].loop[0], &edit->vertex_array[edit->loops[j].loop[0] * 2], edit->loops[i].loop, edit->loops[i].loop_count, edit->vertex_array))
							edit->loops[j].used = FALSE;
			}
		}
//		edit->loops[i].used = mo_edit_process_loop_direction(edit->loops[i].loop, edit->loops[i].loop_count, edit->vertex_array);
	}
}



unsigned int mo_edit_loop_connect_intersect_test(MoEditProcess *edit, unsigned int other_point, unsigned int target_vertex, unsigned int *loop, unsigned int loop_count)
{
	float *a, *b, *point_a, *point_b, vector[2], v[2], x;
	unsigned int i, ii;
	point_a = &edit->vertex_array[other_point * 2];
	x = point_a[0];
	point_b = &edit->vertex_array[target_vertex * 2];
	vector[0] = point_b[0] - point_a[0];
	vector[1] = point_b[1] - point_a[1];
	for(i = 0; i < loop_count; i++)
	{
		ii = (i + 1) % loop_count;
		a = &edit->vertex_array[loop[i] * 2];
		b = &edit->vertex_array[loop[ii] * 2];
		if(a[0] > x || b[0] > x)
		{
			if((a[0] - point_a[0]) * vector[1] - (a[1] - point_a[1]) * vector[0] > 0)
			{
				if((b[0] - point_a[0]) * vector[1] - (b[1] - point_a[1]) * vector[0] < 0)
				{
					v[0] = b[0] - a[0];
					v[1] = b[1] - a[1];
					if((point_a[0] - a[0]) * v[1] - (point_a[1] - a[1]) * v[0] < 0)
					{
						if((point_b[0] - a[0]) * v[1] - (point_b[1] - a[1]) * v[0] > 0)
						{
							if(loop[i] != target_vertex && loop[ii] != target_vertex)
							{
								f_intersect2f(vector, a, b, point_a, point_b);
								if(a[0] > b[0])
									return i;
								else
									return ii;
							}
						}
					}
				}
			}
		}
	}
	return -1;
}


unsigned int mo_edit_loop_merge_loops(MoEditProcess *edit, unsigned int *ref, unsigned int *outer, unsigned int outer_count, unsigned int *inner, unsigned int inner_count, unsigned int inner_start, int direction)
{
	float x, y, tmp, vec[2], vec1[2], vec2[2], best = 100000000000;
	unsigned int i, found = -1, id = 0, count;
	x  =  edit->vertex_array[inner[inner_start] * 2 + 0];
	y  =  edit->vertex_array[inner[inner_start] * 2 + 1];

	for(i = 0; i < outer_count; i++)
	{
		tmp = edit->vertex_array[outer[i] * 2 + 0];
		if(tmp >= x)
		{
			vec[0] = x - tmp;
			vec[1] = (y - edit->vertex_array[outer[i] * 2 + 1]) / 2.0;
			tmp = vec[0] * vec[0] + vec[1] * vec[1];

			if(tmp < best)
			{
				best = tmp;
				found = i;
			}
		}
	}

	i = found;
	while(i != -1)
	{
		found = i;
		i = mo_edit_loop_connect_intersect_test(edit, inner[inner_start], outer[found], outer, outer_count);
	}

	best = 10000000;
	for(i = 0; i < outer_count; i++)
	{
		if(outer[found] == outer[i])
		{
			vec[0] = edit->vertex_array[outer[i] * 2 + 0];
			vec[1] = edit->vertex_array[outer[i] * 2 + 1];
			vec1[0] = edit->vertex_array[outer[(i + 1) % outer_count] * 2 + 0] - vec[0];
			vec1[1] = edit->vertex_array[outer[(i + 1) % outer_count] * 2 + 1] - vec[1];
			f_normalize2f(vec1);
			vec2[0] = edit->vertex_array[outer[(i + outer_count - 1) % outer_count] * 2 + 0] - vec[0];
			vec2[1] = edit->vertex_array[outer[(i + outer_count - 1) % outer_count] * 2 + 1] - vec[1];
			f_normalize2f(vec2);
			vec[0] = vec[0] - x + vec1[0] + vec2[0];
			vec[1] = vec[1] - y + vec1[1] + vec2[1];
			tmp = vec[0] * vec[0] + vec[1] * vec[1];
			if(tmp < best)
			{
				if(found != i)
					found = i;
				best = tmp;
			}
		}
	}

	
	for(i = 0; i < outer_count; i++)
		ref[id++] = outer[(i + found) % outer_count];

	if(direction)
	{
		if(outer[found] != inner[(i + inner_start) % inner_count])
			ref[id++] = outer[found];
		for(i = 0; i < inner_count; i++)
			ref[id++] = inner[(i + inner_start) % inner_count];

		if(ref[0] != inner[inner_start])
			ref[id++] = inner[inner_start];
	}else
	{
		ref[id++] = outer[found];
		if(outer[found] != inner[inner_start])
			ref[id++] = inner[inner_start];
		for(i = inner_count; i > 1; i--)
			ref[id++] = inner[(i - 1 + inner_start) % inner_count];
		if(ref[0] != inner[(i - 1 + inner_start) % inner_count])
			ref[id++] = inner[(i - 1 + inner_start) % inner_count];
	}
	for(i = 0; i < id; i++)
		if(ref[i] == ref[(i + 1) % id])
			i = 0;

	if(outer_count + inner_count + 2 != id)
		i = 0;
	return id;
}


void mo_edit_loop_remove_inside(MoEditProcess *edit)
{
	unsigned int i, j, k;
	float vec[2], *p, *p2, point[2], f;
	for(i = 0; i < edit->loop_count; i++)
		edit->loops[i].used = FALSE;
	for(i = 0; i < edit->loop_count; i++)
		if(edit->loops[i].material != -1)
			for(j = 0; j < edit->loop_count; j++)
				if(edit->loops[j].material == edit->loops[j].material && !edit->loops[i].used && !edit->loops[j].used)
					if(mo_edit_process_inside_test(edit->loops[j].loop[0], &edit->vertex_array[edit->loops[j].loop[0] * 2], edit->loops[i].loop, edit->loops[i].loop_count, edit->vertex_array))
					{
						p = &edit->vertex_array[edit->loops[j].loop[0] * 2];
						p2 = &edit->vertex_array[edit->loops[j].loop[1] * 2];
						vec[0] = p[0] - p2[0];
						vec[1] = p[1] - p2[1];
						f = sqrt(vec[0] * vec[0] + vec[1] + vec[1]);
						point[0] = (p[0] + p2[0]) * 0.5 + vec[1] / f * MO_EDIT_PROCESS_COLAPSE_SIZE;
						point[1] = (p[1] + p2[1]) * 0.5 - vec[0] / f * MO_EDIT_PROCESS_COLAPSE_SIZE;
						if(mo_edit_process_inside_test(-1, point, edit->loops[j].loop, edit->loops[j].loop_count, edit->vertex_array))
							edit->loops[j].used = TRUE;
					}
}


/*
		if(edit->loops[i].used && !mo_edit_process_loop_direction(edit->loops[i].loop, edit->loops[i].loop_count, edit->vertex_array)) 
		{
			generation = 0; 
			for(j = 0; j < edit->loop_count; j++)
				if(edit->loops[i].material == edit->loops[j].material)
					if(mo_edit_process_inside_test(edit->loops[i].loop[0], &edit->vertex_array[edit->loops[i].loop[0] * 2], edit->loops[j].loop, edit->loops[j].loop_count, edit->vertex_array))
						generation++;
*/

void mo_edit_loop_connect(MoEditProcess *edit)
{
	unsigned int i, j, k, *holes, hole_count, *loop, start, size, **original_loops, *original_loop_size, *original_loop_material, original_loop_count = 0;
	float best, test[2];
	int first;
	holes = malloc((sizeof  *holes) * edit->loop_count);
	original_loops = malloc((sizeof  *original_loops) * edit->loop_count);
	original_loop_size = malloc((sizeof  *original_loop_size) * edit->loop_count);
	original_loop_material = malloc((sizeof  *original_loop_material) * edit->loop_count);

	for(i = 0; i < edit->loop_count; i++)
		edit->loops[i].direction = mo_edit_process_loop_direction(edit->loops[i].loop, edit->loops[i].loop_count, edit->vertex_array);


	for(i = 0; i < edit->loop_count; i++)
	{
		best = edit->vertex_array[edit->loops[i].loop[0] * 2];
		edit->loops[i].start = 0;
		for(j = 1; j < edit->loops[i].loop_count; j++)
		{
			if(best < edit->vertex_array[edit->loops[i].loop[j] * 2])
			{
				best = edit->vertex_array[edit->loops[i].loop[j] * 2];
				edit->loops[i].start = j;
			}
		}

	}

	for(i = 0; i < edit->loop_count; i++)
	{
		edit->loops[i].used = FALSE;
		edit->loops[i].generation = 0;
	}

	for(i = 0; i < edit->loop_count; i++)
	{
	//	printf("[%u] material %u %u lenght %u\n", i, edit->loops[i].material, -1, edit->loops[i].loop_count);

		if(!edit->loops[i].used && !edit->loops[i].direction)
		{
			hole_count = 0;
			for(j = 0; j < edit->loop_count; j++)
				if(!edit->loops[j].used && edit->loops[i].material == edit->loops[j].material)
					if(mo_edit_process_inside_test(edit->loops[j].loop[0], &edit->vertex_array[edit->loops[j].loop[0] * 2], edit->loops[i].loop, edit->loops[i].loop_count, edit->vertex_array))
						holes[hole_count++] = j;

			for(j = 0; j < hole_count; j++)
			{
				for(k = 0; k < hole_count; k++)
				{
					if(j != k && mo_edit_process_inside_test(edit->loops[holes[k]].loop[0], &edit->vertex_array[edit->loops[holes[k]].loop[0] * 2], edit->loops[holes[j]].loop, edit->loops[holes[j]].loop_count, edit->vertex_array))
					{
						holes[k--] = holes[--hole_count];
						break;
					}
				}
			}
			for(j = 0; j < original_loop_count; j++)
			{
				if(mo_edit_process_inside_test(original_loops[j][0], &edit->vertex_array[original_loops[j][0] * 2], edit->loops[i].loop, edit->loops[i].loop_count, edit->vertex_array))
				{
					for(k = 0; k < hole_count; k++)
					{
						if(j != k && mo_edit_process_inside_test(edit->loops[holes[k]].loop[0], &edit->vertex_array[edit->loops[holes[k]].loop[0] * 2], original_loops[j], original_loop_size[j], edit->vertex_array))
						{
							holes[k--] = holes[--hole_count];
							break;
						}
					}
				}
			}
		/*	for(j = 0; j < hole_count; j++)
			{

						free(original_loops[i]);
					free(original_loops);
					free(original_loop_size);*/


		//	printf("holes found %u\n", hole_count);
			first = TRUE;
			while(hole_count > 0)
			{
				k = 0;
				for(j = 1; j < hole_count; j++)
					if(edit->vertex_array[edit->loops[holes[k]].loop[edit->loops[holes[k]].start] * 2] < edit->vertex_array[edit->loops[holes[j]].loop[edit->loops[holes[j]].start] * 2])
						k = j;

				first = FALSE;
				test[0] = edit->vertex_array[edit->loops[holes[k]].loop[edit->loops[holes[k]].start] * 2];
				test[1] = edit->vertex_array[edit->loops[holes[k]].loop[edit->loops[holes[k]].start] * 2 + 1];


				loop = malloc((sizeof *loop) * (edit->loops[i].loop_count + 2 + edit->loops[holes[k]].loop_count));
				size = mo_edit_loop_merge_loops(edit, loop, edit->loops[i].loop, edit->loops[i].loop_count, 
													edit->loops[holes[k]].loop, edit->loops[holes[k]].loop_count, edit->loops[holes[k]].start, edit->loops[holes[k]].direction);
				original_loops[original_loop_count] = edit->loops[i].loop;
				original_loop_material[original_loop_count] = edit->loops[i].material;
				original_loop_size[original_loop_count++] = edit->loops[i].loop_count;
				edit->loops[i].loop = loop;
				edit->loops[i].loop_count = size;
				edit->loops[holes[k]].used = TRUE;
				holes[k] = holes[--hole_count];
			}
		}
	}
	for(i = 0; i < original_loop_count; i++)
		free(original_loops[i]);
	free(original_loops);
	free(original_loop_size);
	free(holes);
}


void mo_edit_loop_polygonize(MoEditProcess *edit, unsigned int *loop, unsigned int size, unsigned int *ref)
{
	unsigned int i = 0, a = 0, b = 1, c = 2, vertex, count = 2, found[3], output = 0, ref_length = 0, save[2];
	float *array, vec[2], *v, *base, *back, sides[4], f, dist, best;
	array = edit->vertex_array;
	for(i = 0; i < (size - 2) * 3; i++)
		ref[i] = 0;
	while(count < size)
	{
		save[1] = -1;
		if(count + 1 < size)
		{
			best = 1000000;
			for(i = 0; i < size; i++)
			{
				a = b;
				b = c;
				c = (c + 1) % size;
				while(loop[c] == -1)
					c = (c + 1) % size;

				base = &array[loop[c] * 2];
				vec[0] = array[loop[a] * 2 + 0] - base[0];
				vec[1] = array[loop[a] * 2 + 1] - base[1];
				f_normalize2f(vec);

				back = &array[loop[b] * 2 + 0];
				dist = (vec[1] * (back[0] - base[0]) - vec[0] * (back[1] - base[1]));

				sides[0] = array[loop[a] * 2 + 0] - back[0];
				sides[1] = array[loop[a] * 2 + 1] - back[1];
				sides[2] = array[loop[c] * 2 + 0] - back[0];
				sides[3] = array[loop[c] * 2 + 1] - back[1];
				save[0] = -1;
				for(vertex = (c + 1) % size; vertex != a; vertex = (vertex + 1) % size)
				{
					if(loop[vertex] != -1)
					{
						v = &array[loop[vertex] * 2];
						if(0 <= sides[1] * (v[0] - back[0]) - sides[0] * (v[1] - back[1]) &&
							0 >= sides[3] * (v[0] - back[0]) - sides[2] * (v[1] - back[1]))
						{
							f = (vec[1] * (base[0] - v[0]) - vec[0] * (base[1] - v[1]));
							if(f > dist)
							{
								if(f < 0.0 || (0 < sides[1] * (v[0] - back[0]) - sides[0] * (v[1] - back[1]) && 0 > sides[3] * (v[0] - back[0]) - sides[2] * (v[1] - back[1])))
								{
									save[0] = loop[vertex];
									dist = f;
								}
							}
						}
					}
				}
				if(dist < best)
				{
					save[1] = save[0];
					best = dist;
					found[0] = a;
					found[1] = b;
					found[2] = c;
				}
			}
			a = found[0];
			b = found[1];
			c = found[2];
		}

		ref[ref_length++] = loop[a];
		ref[ref_length++] = loop[b];
		ref[ref_length++] = loop[c];
		loop[b] = -1;
		b = c;
		c = (c + 1) % size;
		while(loop[c] == -1)
			c = (c + 1) % size;
		count++;
	}
}


void mo_edit_process_polygon_turn(unsigned int *neighbor, unsigned int *reference, unsigned int polygon, unsigned int edge)
{
	unsigned int n_poly, n_edge, tmp;
	n_poly = neighbor[polygon * 3 + edge];
	n_edge = n_poly % 3;
	n_poly = n_poly / 3;
	tmp = reference[polygon * 3 + (edge + 2) % 3];
	reference[polygon * 3 + (edge + 1) % 3] = reference[n_poly * 3 + (n_edge + 2) % 3];
	reference[n_poly * 3 + (n_edge + 1) % 3] = tmp;

	tmp = neighbor[polygon * 3 + (edge + 1) % 3];
	if(tmp != -1)
	{
		neighbor[tmp] = n_poly * 3 + (n_edge + 0) % 3;
		neighbor[neighbor[tmp]] = tmp;
	}else
		neighbor[n_poly * 3 + (n_edge + 0) % 3] = -1;
	
	tmp = neighbor[n_poly * 3 + (n_edge + 1) % 3];
	if(tmp != -1)
	{
		neighbor[tmp] = polygon * 3 + (edge + 0) % 3;
		neighbor[neighbor[tmp]] = tmp;
	}else
		neighbor[polygon * 3 + (edge + 0) % 3] = -1;

	tmp = n_poly * 3 + (n_edge + 1) % 3;
	neighbor[tmp] = polygon * 3 + (edge + 1) % 3;
	neighbor[neighbor[tmp]] = tmp;				
}

#define MOON_FIXED_POINT 1024

int mo_edit_process_polygon_turn_test(unsigned int *neighbor, unsigned int *reference, int *vertexi, unsigned char *poly_type, unsigned int polygon, unsigned int edge)
{
	int *corners[4], edge_vec[4][2], center_vec[2][2], n, tmp, a, b;
	if(neighbor[polygon * 3 + edge] == -1 ||
		poly_type[polygon] != poly_type[neighbor[polygon * 3 + edge] / 3])
		return FALSE;
	
	corners[0] = &vertexi[reference[polygon * 3 + (edge + 0) % 3] * 3];
	n = neighbor[polygon * 3 + edge];
	corners[1] = &vertexi[reference[(n / 3) * 3 + (n + 2) % 3] * 3];
	corners[2] = &vertexi[reference[polygon * 3 + (edge + 1) % 3] * 3];
	corners[3] = &vertexi[reference[polygon * 3 + (edge + 2) % 3] * 3];
	
	edge_vec[0][0] = corners[1][0] - corners[0][0];
	edge_vec[0][1] = corners[1][2] - corners[0][2];
	edge_vec[1][0] = corners[2][0] - corners[1][0];
	edge_vec[1][1] = corners[2][2] - corners[1][2];
	edge_vec[2][0] = corners[3][0] - corners[2][0];
	edge_vec[2][1] = corners[3][2] - corners[2][2];
	edge_vec[3][0] = corners[0][0] - corners[3][0];
	edge_vec[3][1] = corners[0][2] - corners[3][2];

	center_vec[0][0] = corners[2][0] - corners[0][0];
	center_vec[0][1] = corners[2][2] - corners[0][2];

	center_vec[1][0] = corners[3][0] - corners[1][0];
	center_vec[1][1] = corners[3][2] - corners[1][2];
	f_normalize_2di(center_vec[0], MOON_FIXED_POINT); 
	f_normalize_2di(center_vec[1], MOON_FIXED_POINT); 


	
	if((corners[0][0] - corners[1][0]) * center_vec[1][1] - (corners[0][2] - corners[1][2]) * center_vec[1][0] > 0)
		return FALSE;
	if((corners[2][0] - corners[1][0]) * center_vec[1][1] - (corners[2][2] - corners[1][2]) * center_vec[1][0] < 0)
		return FALSE;

	f_normalize_2di(edge_vec[0], MOON_FIXED_POINT); 
	f_normalize_2di(edge_vec[1], MOON_FIXED_POINT); 
	f_normalize_2di(edge_vec[2], MOON_FIXED_POINT); 
	f_normalize_2di(edge_vec[3], MOON_FIXED_POINT); 
	
	/* need convex test here*/

	a = edge_vec[0][0] * center_vec[0][0] + edge_vec[0][1] * center_vec[0][1];
	tmp = -edge_vec[3][0] * center_vec[0][0] + -edge_vec[3][1] * center_vec[0][1];
	if(tmp > a)
		a = tmp;
	tmp = -edge_vec[1][0] * -center_vec[0][0] + -edge_vec[1][1] * -center_vec[0][1];
	if(tmp > a)
		a = tmp;
	tmp = edge_vec[2][0] * -center_vec[0][0] + edge_vec[2][1] * -center_vec[0][1];
	if(tmp > a)
		a = tmp;


	b = -edge_vec[0][0] * center_vec[1][0] + -edge_vec[0][1] * center_vec[1][1];
//	b = edge_vec[0][0] * center_vec[1][0] + edge_vec[0][1] * center_vec[1][1];
	tmp = edge_vec[1][0] * center_vec[1][0] + edge_vec[1][1] * center_vec[1][1];
//	tmp = -edge_vec[3][0] * center_vec[1][0] + -edge_vec[3][1] * center_vec[1][1];
	if(tmp > b)
		b = tmp;

	tmp = edge_vec[3][0] * -center_vec[1][0] + edge_vec[3][1] * -center_vec[1][1];
//	tmp = -edge_vec[1][0] * -center_vec[1][0] + -edge_vec[1][1] * -center_vec[1][1];
	if(tmp > b)
		b = tmp;
	tmp = -edge_vec[2][0] * -center_vec[1][0] + -edge_vec[2][1] * -center_vec[1][1];
//	tmp = edge_vec[2][0] * -center_vec[1][0] + edge_vec[2][1] * -center_vec[1][1];
	if(tmp > b)
		b = tmp;

	if(a > b)
	{
		mo_edit_process_polygon_turn(neighbor, reference, polygon, edge);
		return TRUE;
	}
	return FALSE;
}

void mo_edit_process_polygon_turn_all(unsigned int *neighbor, unsigned int *reference, int *vertexi, unsigned char *poly_type, unsigned int tri_length)
{
	int *buffer, found;
	unsigned int i, j, k, next, poly_count;
	poly_count = tri_length / 3;
	buffer = malloc((sizeof *buffer) * poly_count);
	for(j = 0; j < poly_count; j++)
		buffer[j] = TRUE;
	for(i = 0; i < poly_count;)
	{
		for(j = 0; j < poly_count; j++)
		{
			i++;
			if(buffer[j])
			{
				found = FALSE;
				for(k = 0; k < 3; k++)
				{
					next = neighbor[j * 3 + k];
					if(mo_edit_process_polygon_turn_test(neighbor, reference, vertexi, poly_type, j, k))
					{
						buffer[next / 3] = TRUE;
						buffer[j] = TRUE;
						i = 0;
					}						
				}
			}
		}
	}
	free(buffer);
}

void mo_edit_process_polygonize(MoEditProcess *edit)
{
	unsigned int i, j, size, *ref, *neighbor, building = MO_LST_FACTORY_UNASIGNED;
	int *vertex_array;
	unsigned char *type;
	for(i = size = 0; i < edit->loop_count; i++)
		if(!edit->loops[i].direction && !edit->loops[i].used && edit->loops[i].material != -1)
			size += (edit->loops[i].loop_count - 2) * 3;
	ref = malloc((sizeof *ref) * size);
	type  = malloc((sizeof *type) * size / 3);
	for(i = size = 0; i < edit->loop_count; i++)
	{
		if(!edit->loops[i].direction && !edit->loops[i].used && edit->loops[i].material != -1)
		{
			for(j = 0; j < edit->loops[i].loop_count; j++)
				if(edit->loops[i].loop[j] == edit->loops[i].loop[(j + 1) % edit->loops[i].loop_count])
					j = 0;
			mo_edit_loop_polygonize(edit, edit->loops[i].loop, edit->loops[i].loop_count, &ref[size]);

			for(j = 0; j < (edit->loops[i].loop_count - 2); j++)
				type[size / 3 + j] = edit->loops[i].material;
			size += (edit->loops[i].loop_count - 2) * 3;
		}
	}
	
	edit->ref_count = size;
	edit->ref = ref;
	edit->material = type;
	return;
}




void mo_edit_process_deploy(float *vertex_array, unsigned int *loop_sizes, unsigned int loop_count, ProcessOutput *output)
{
	MoEditProcess *edit;
	float offset[2] = {0, 0};
	edit = mo_edit_process_init(vertex_array, loop_sizes, loop_count);
	mo_edit_reloop(edit);
	mo_edit_process_vertex_dominance2(edit);
	mo_edit_process_reduce(edit);
	mo_edit_process_loop_extract(edit, -1);
//	mo_edit_process_test_draw(edit);
	mo_edit_loop_connect(edit);
//	mo_edit_process_loop_draw(edit);
	mo_edit_process_polygonize(edit);
	output->vertex_array = edit->vertex_array;
	output->vertex_count = edit->vertex_count;
	output->ref_count = edit->ref_count;
	output->ref = edit->ref;
	output->material = edit->material;
	free(edit->edges);
	free(edit->vertex_users);
	free(edit->loops);
	free(edit);

}

