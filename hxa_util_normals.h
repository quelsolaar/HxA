
/* Do not include or call anything defined in this .h file. It is exclusivly for the use of hxa_util_normals.c */
#ifdef NORMAL_FACE

HXATYPE *hxa_util_normal_face_macro(HXANode *node)
{
	HXATYPE v1[3], v2[3], f, *normals, *n, *vertex, *a, *b, *c;
	unsigned int i, j, count;
	int *ref;
	ref = node->content.geometry.corner_stack.layers[HXA_CONVENTION_HARD_BASE_CORNER_LAYER_ID].data.int32_data;
	vertex = node->content.geometry.vertex_stack.layers[HXA_CONVENTION_HARD_BASE_VERTEX_LAYER_ID].data.HXATYPE_DATA;
	n = normals = malloc((sizeof *normals) * node->content.geometry.face_count * 3);
	for(i = 0; i < node->content.geometry.edge_corner_count;)
	{
		if(ref[i + 2] < 0) /* Triangle */
		{
			a = &vertex[hxa_ref(ref, i) * 3];
			b = &vertex[hxa_ref(ref, i + 1) * 3];
			c = &vertex[hxa_ref(ref, i + 2) * 3];
			v1[0] = a[0] - c[0];
			v1[1] = a[1] - c[1];
			v1[2] = a[2] - c[2];
			v2[0] = b[0] - c[0];
			v2[1] = b[1] - c[1];
			v2[2] = b[2] - c[2];
			n[0] = v1[2] * v2[1] - v1[1] * v2[2];
			n[1] = v1[0] * v2[2] - v1[2] * v2[0];
			n[2] = v1[1] * v2[0] - v1[0] * v2[1];
			i += 3;
		}else
		{
			count = node->content.geometry.edge_corner_count - i;
			n[0] = n[1] = n[2] = 0;
			for(j = 0; j < count; j++)
			{
				if(ref[i + j + 2] < 0)
					count = j + 2;
				a = &vertex[hxa_ref(ref, (i + j)) * 3];
				b = &vertex[hxa_ref(ref, (i + (j + 1) % count)) * 3];
				c = &vertex[hxa_ref(ref, (i + (j + 2) % count)) * 3];
				v1[0] = a[0] - c[0];
				v1[1] = a[1] - c[1];
				v1[2] = a[2] - c[2];
				v2[0] = b[0] - c[0];
				v2[1] = b[1] - c[1];
				v2[2] = b[2] - c[2];
				n[0] += v1[2] * v2[1] - v1[1] * v2[2];
				n[1] += v1[0] * v2[2] - v1[2] * v2[0];
				n[2] += v1[1] * v2[0] - v1[0] * v2[1];
			}
			i += count + 1;
		}
		f = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
		n[0] /= f;
		n[1] /= f;
		n[2] /= f;
		n += 3;
	}
	return normals;
}

#endif



#ifdef NORMAL_CORNER

HXATYPE *hxa_util_normal_corner_macro(HXANode *node, 
								unsigned int *neighburs,		
#ifdef HXA_CREASE_TYPE
								HXA_CREASE_TYPE *creases, 
#endif
								HXATYPE *face_normals,
								unsigned int *faces)
{
	HXATYPE *corner_normals, *n, tmp[3], f;
	unsigned int i, r;
	int * ref;
	ref = node->content.geometry.corner_stack.layers[HXA_CONVENTION_HARD_BASE_CORNER_LAYER_ID].data.int32_data;
	corner_normals = malloc((sizeof *corner_normals) * node->content.geometry.edge_corner_count * 3);
	for(i = 0; i < node->content.geometry.edge_corner_count * 3; i++)
		corner_normals[i] = 0;
	for(i = 0; i < node->content.geometry.edge_corner_count; i++)
	{
		if(corner_normals[i * 3] == 0 && corner_normals[i * 3 + 1] == 0 && corner_normals[i * 3 + 2] == 0)
		{
			r = i;
			tmp[0] = tmp[1] = tmp[2] = 0;
			n = &face_normals[faces[r] * 3];
			tmp[0] += n[0];
			tmp[1] += n[1];
			tmp[2] += n[2];

			r = i;
			while(r = neighburs[hxa_corner_get_previous(ref, r)] != -1 && r != i
#ifdef HXA_CREASE_TYPE							
					&& !creases[r]
#endif	
				)
			{
				n = &face_normals[faces[r] * 3];
				tmp[0] += n[0];
				tmp[1] += n[1];
				tmp[2] += n[2];
			}
			if(i != r)
			{
				r = i;
				while(
#ifdef HXA_CREASE_TYPE							
						!creases[r] && 
#endif	
					(r = neighburs[r]) != -1 && (r = hxa_corner_get_next(ref, r)) != i)
				{
						
					n = &face_normals[faces[r] * 3];
					tmp[0] += n[0];
					tmp[1] += n[1];
					tmp[2] += n[2];
				}
			}
			f = sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]);
			tmp[0] /= f;
			tmp[1] /= f;
			tmp[2] /= f;
			
			corner_normals[i * 3] = tmp[0];
			corner_normals[i * 3 + 1] = tmp[1];
			corner_normals[i * 3 + 2] = tmp[2];

			r = i;
			while(r = neighburs[hxa_corner_get_previous(ref, r)] != -1 && r != i
#ifdef HXA_CREASE_TYPE							
					&& !creases[r]
#endif	
				)
			{
				corner_normals[r * 3] = tmp[0];
				corner_normals[r * 3 + 1] = tmp[1];
				corner_normals[r * 3 + 2] = tmp[2];
			}
			if(i != r)
			{			
				r = i;
				while(
#ifdef HXA_CREASE_TYPE							
						!creases[r] && 
#endif	
					(r = neighburs[r]) != -1 && (r = hxa_corner_get_next(ref, r)) != i)
				{
						
					corner_normals[r * 3] = tmp[0];
					corner_normals[r * 3 + 1] = tmp[1];
					corner_normals[r * 3 + 2] = tmp[2];
				}
			}
		}
	}
	return corner_normals;
}
#endif

