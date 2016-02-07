/*
 *
 *	CLASS Clause 
 *
*/

// STD INCLUDES
#include <vector>	//std::vector
#include <pair>		//std::pair
class Clause
{
	public:
		Clause(const std::vector<int> &p_literals);
		~Clause();

		bool isSolvable();
		bool solve();

		std::vector<std::pair<int, bool>> getValuation();

	private:
		std::vector<std::pair<int, bool>> m_literals;
		std::vecotr<std::pair<int, bool>> m_valuation;
}; // Clause

