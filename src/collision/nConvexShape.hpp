#include <nNewton/nCollisionShapes.hpp>
#include <vector>


namespace nNewton
{

	class nConvexShape : public nCollisionShape
	{
	public:
		struct Face_ConvexShape
		{
			std::vector<int> vert_indices;
			nVector3 faceNormal;
		};

		struct Edge_ConvexShape
		{
			int vert1;
			int vert2;
			int adjface1;
			int adjface2;
		};
	
	private:

		std::vector <nVector3> m_verts;
		std::vector<Face_ConvexShape> m_faces;
		std::vector<Edge_ConvexShape> m_edges;
		std::vector<std::vector<int>> m_adj_vert_indices;
	
	public:
		
		const std::vector<nVector3>& GetVerts() const { return m_verts; }
		const std::vector<Face_ConvexShape>& GetFaces() const { return m_faces; }
		const std::vector<Edge_ConvexShape>& GetEdges() const { return m_edges; }


		nAABB getAABB(const nTransform& transform_)const override
		{ 
			nVector3 mn = m_verts[0], mx = m_verts[0];
			for (const auto& v : m_verts) {
				mn = Min(mn, v);
				mx = Max(mx, v);
			}
			return nAABB(mn, mx);

		}

		nVector3 getSupportPoint(const nVector3& direction)const override
		{
			// hill climbing implementation
			float bestPoint = DotProduct(m_verts[0], direction);
			int bestIndx = 0;
			bool find = true;

			while (find)
			{
				find = false;
				int j = 0;
					 
				while (m_adj_vert_indices[bestIndx][j] != -1)
				{
					if (bestPoint < DotProduct(m_verts[m_adj_vert_indices[bestIndx][j]], direction))
					{
						bestPoint = DotProduct(m_verts[m_adj_vert_indices[bestIndx][j]], direction);
						bestIndx = m_adj_vert_indices[bestIndx][j];
						find = true;
						
					}
					
					++j;
				}	
			}
			return m_verts[bestIndx];
		}


	};

	
}
