package weka.classifiers.trees.j48;

import weka.core.RevisionUtils;
import weka.core.Utils;

/**
 * Class for computing the information gain for a given distribution.
 *
 * @author Ryan Lichtenwalter (rlichten@cse.nd.edu)
 * @version $Revision: 1.00 $
 */
public final class HTreeSplitCrit extends SplitCriterion {

	/**
	 * This method is a straightforward implementation of the hellinger distance
	 * criterion for the given distribution.
	 *
	 * @param bags the distribution
	 */
	public final double splitCritValue( Distribution bags ) {
		double n = bags.actualNumClasses();
		double distance = 0;
		for( int classIndex1 = 0; classIndex1 < n; classIndex1++ ) {
			for( int classIndex2 = 0; classIndex2 < n; classIndex2++ ) {
				if( classIndex1 == classIndex2 ) {
					continue;
				}
				double classCount1 = bags.perClass( classIndex1 );
				double classCount2 = bags.perClass( classIndex2 );

				double tempDistance = 0;
				for( int bagIndex = 0; bagIndex < 2; bagIndex++ ) {
					double classByBagCount1 = bags.perClassPerBag( bagIndex, classIndex1 );
					double classByBagCount2 = bags.perClassPerBag( bagIndex, classIndex2 );
					double percInBag1 = classByBagCount1 / classCount1;
					double percInBag2 = classByBagCount2 / classCount2;
					double rootDifference = Math.sqrt( percInBag1 ) - Math.sqrt( percInBag2 );
					tempDistance += Math.pow( rootDifference, 2 );
				}
				distance += Math.sqrt( tempDistance );
			}
		}
		
		// for multi-class support
		distance = distance / ( n * ( n - 1 ) / 2 );

		// Splits with no distance are useless.
		if( Utils.eq( distance, 0 ) ) {
			return Double.NEGATIVE_INFINITY;
		}
		
		return distance;
	}

	/**
	 * This method is a straightforward implementation of the Hellinger distance
	 * criterion for the given distribution.
	 *
	 * @param bags the distribution
	 * @param totalNoInst weight of ALL instances (including the ones with missing values).
	 */
	public final double splitCritValue( Distribution bags, double totalNoInst ) {
		double noUnknown = totalNoInst - bags.total();
		double unknownRate = noUnknown / totalNoInst;
		double distance = splitCritValue( bags );

		return (1-unknownRate) * distance;
	}

	/**
	 * Returns the revision string.
	 * 
	 * @return the revision
	 */
	public String getRevision() {
		return RevisionUtils.extract( "$Revision: 1.00 $" );
	}
}
