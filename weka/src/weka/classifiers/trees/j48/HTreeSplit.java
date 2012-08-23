package weka.classifiers.trees.j48;

import weka.core.Instance;
import weka.core.Instances;
import weka.core.RevisionUtils;
import weka.core.Utils;

import java.util.Enumeration;

/**
 * Class implementing a Hellinger tree split on an attribute.
 *
 * @author Ryan Lichtenwalter (rlichten@cse.nd.edu)
 * @version $Revision: 1.00 $
 */
public class HTreeSplit extends ClassifierSplitModel {

	/** Desired number of branches. */
	private int m_complexityIndex;
	/** Attribute to split on. */
	private int m_attIndex;
	/** Minimum number of objects in a split.   */
	private int m_minNoObj;
	/** Value of split point. */
	private double m_splitPoint;
	/** Hellinger distance of split.  */
	private double m_hellingerDistance;
	/** The sum of the weights of the instances. */
	private double m_sumOfWeights;
	/** Number of split points. */
	private int m_index;

	/**
	 * Initializes the split model.
	 */
	public HTreeSplit( int attIndex, int minNoObj, double sumOfWeights ) {
		// Get index of attribute to split on.
		m_attIndex = attIndex;

		// Set minimum number of objects.
		m_minNoObj = minNoObj;

		// Set the sum of the weights
		m_sumOfWeights = sumOfWeights;
	}

	/**
	 * Creates a Hellinger tree split on the given data. Assumes that none of
	 * the class values is missing.
	 *
	 * @exception Exception if something goes wrong
	 */
	public void buildClassifier( Instances trainInstances ) throws Exception {
		// Initialize the remaining instance variables.
		m_numSubsets = 0;
		m_splitPoint = Double.MAX_VALUE;
		m_hellingerDistance = 0;

		// Different treatment for enumerated and numeric
		// attributes.
		if( trainInstances.attribute( m_attIndex ).isNominal() ) {
			m_complexityIndex = trainInstances.attribute( m_attIndex ).numValues();
			m_index = m_complexityIndex;
			handleEnumeratedAttribute( trainInstances );
		} else {
			m_complexityIndex = 2;
			m_index = 0;
			trainInstances.sort( trainInstances.attribute( m_attIndex ) );
			handleNumericAttribute( trainInstances );
		}
	}

	/**
	 * Returns index of attribute for which split was generated.
	 */
	public final int attIndex() {
		return m_attIndex;
	}

	/**
	 * Gets class probability for instance.
	 *
	 * @exception Exception if something goes wrong
	 */
	public final double classProb( int classIndex, Instance instance, int theSubset ) throws Exception {
		if( theSubset <= -1 ) {
			double[] weights = weights( instance );
			if( weights == null ) {
				return m_distribution.prob( classIndex );
			} else {
				double prob = 0;
				for( int i = 0; i < weights.length; i++ ) {
					prob += weights[i] * m_distribution.prob( classIndex, i );
				}
				return prob;
			}
		} else {
			if( Utils.gr( m_distribution.perBag( theSubset ), 0 ) ) {
				return m_distribution.prob( classIndex, theSubset );
			} else {
				return m_distribution.prob( classIndex );
			}
		}
	}

	/**
	 * Returns coding cost for split (used in rule learner).
	 */
	public final double codingCost() {
		return Utils.log2( m_index );
	}

	/**
	 * Returns Hellinger distance for the generated split.
	 */
	public final double hellingerDistance() {
		return m_hellingerDistance;
	}

	/**
	 * Creates split on enumerated attribute.
	 *
	 * @exception Exception if something goes wrong
	 */
	private void handleEnumeratedAttribute( Instances trainInstances ) throws Exception {
		m_distribution = new Distribution( m_complexityIndex, trainInstances.numClasses() );

		// Only Instances with known values are relevant.
		Enumeration instanceEnum = trainInstances.enumerateInstances();
		while( instanceEnum.hasMoreElements() ) {
			Instance instance = (Instance) instanceEnum.nextElement();
			if( !instance.isMissing( m_attIndex ) ) {
				m_distribution.add( (int) instance.value( m_attIndex ), instance );
			}
		}

		// Check if minimum number of Instances in at least two subsets.
		if( m_distribution.check( m_minNoObj ) ) {
			m_numSubsets = m_complexityIndex;
			HTreeSplitCrit hellingerSplitCrit = new HTreeSplitCrit();
			m_hellingerDistance = hellingerSplitCrit.splitCritValue( m_distribution, m_sumOfWeights );
		}
	}

	/**
	 * Creates split on numeric attribute.
	 *
	 * @exception Exception if something goes wrong
	 */
	private void handleNumericAttribute( Instances trainInstances ) throws Exception {
		// Current attribute is a numeric attribute.
		m_distribution = new Distribution( 2, trainInstances.numClasses() );

		// Only Instances with known values are relevant.
		Enumeration instanceEnum = trainInstances.enumerateInstances();
		int firstMiss = 0;
		while( instanceEnum.hasMoreElements() ) {
			Instance instance = (Instance) instanceEnum.nextElement();
			if( instance.isMissing( m_attIndex ) ) {
				break;
			}
			m_distribution.add( 1, instance );
			firstMiss++;
		}

		// Compute minimum number of Instances required in each subset.
//		double minSplit = 0.1 * ( m_distribution.total() ) / ( (double) trainInstances.numClasses() );
		double minSplit = m_minNoObj;
		if( Utils.smOrEq( minSplit, m_minNoObj ) ) {
			minSplit = m_minNoObj;
		} else if( Utils.gr( minSplit, 25 ) ) {
			minSplit = 25;
		}
		
		// Enough Instances with known values?
		if( Utils.sm( (double) firstMiss, 2 * minSplit ) ) {
			return;
		}
		
		// Compute values of criteria for all possible split indices.
		HTreeSplitCrit hellingerSplitCrit = new HTreeSplitCrit();
		int next = 1;
		int last = 0;
		int splitIndex = -1;
		while( next < firstMiss ) {
			if( trainInstances.instance( next - 1 ).value( m_attIndex ) + 1e-5 < trainInstances.instance( next ).value( m_attIndex ) ) {

				// Move class values for all Instances up to next possible split point.
				m_distribution.shiftRange( 1, 0, trainInstances, last, next );

				// Check if enough Instances in each subset and compute values for criteria.
				if( Utils.grOrEq( m_distribution.perBag( 0 ), minSplit ) && Utils.grOrEq( m_distribution.perBag( 1 ), minSplit ) ) {
					double currentDistance = hellingerSplitCrit.splitCritValue( m_distribution, m_sumOfWeights );
					if( Utils.gr( currentDistance, m_hellingerDistance ) ) {
						m_hellingerDistance = currentDistance;
						splitIndex = next - 1;
					}
					m_index++;
				}
				last = next;
			}
			next++;
		}

		// Was there any useful split?
		if( m_index == 0 || splitIndex == -1 ) {
			return;
		}
		
		m_numSubsets = 2;
		m_splitPoint = trainInstances.instance( splitIndex ).value( m_attIndex );

		// Restore distribution for best split.
		m_distribution = new Distribution( 2, trainInstances.numClasses() );
		m_distribution.addRange( 0, trainInstances, 0, splitIndex + 1 );
		m_distribution.addRange( 1, trainInstances, splitIndex + 1, firstMiss );
	}

	/**
	 * Prints left side of condition.
	 *
	 * @param data training set.
	 */
	public final String leftSide( Instances data ) {
		return data.attribute( m_attIndex ).name();
	}

	/**
	 * Prints the condition satisfied by instances in a subset.
	 *
	 * @param index of subset 
	 * @param data training set.
	 */
	public final String rightSide( int index, Instances data ) {
		StringBuffer text;

		text = new StringBuffer();
		if( data.attribute( m_attIndex ).isNominal() ) {
			text.append( " = " +
					data.attribute( m_attIndex ).value( index ) );
		} else if( index == 0 ) {
			text.append( " <= " +
					Utils.doubleToString( m_splitPoint, 6 ) );
		} else {
			text.append( " > " +
					Utils.doubleToString( m_splitPoint, 6 ) );
		}
		return text.toString();
	}

	/**
	 * Returns a string containing java source code equivalent to the test
	 * made at this node. The instance being tested is called "i".
	 *
	 * @param index index of the nominal value tested
	 * @param data the data containing instance structure info
	 * @return a value of type 'String'
	 */
	public final String sourceExpression( int index, Instances data ) {

		StringBuffer expr = null;
		if( index < 0 ) {
			return "i[" + m_attIndex + "] == null";
		}
		if( data.attribute( m_attIndex ).isNominal() ) {
			expr = new StringBuffer( "i[" );
			expr.append( m_attIndex ).append( "]" );
			expr.append( ".equals(\"" ).append( data.attribute( m_attIndex ).value( index ) ).append( "\")" );
		} else {
			expr = new StringBuffer( "((Double) i[" );
			expr.append( m_attIndex ).append( "])" );
			if( index == 0 ) {
				expr.append( ".doubleValue() <= " ).append( m_splitPoint );
			} else {
				expr.append( ".doubleValue() > " ).append( m_splitPoint );
			}
		}
		return expr.toString();
	}

	/**
	 * Sets split point to greatest value in given data smaller or equal to
	 * old split point.
	 * (C4.5 does this for some strange reason).
	 */
	public final void setSplitPoint( Instances allInstances ) {

		double newSplitPoint = -Double.MAX_VALUE;
		double tempValue;
		Instance instance;

		if( ( allInstances.attribute( m_attIndex ).isNumeric() ) &&
				( m_numSubsets > 1 ) ) {
			Enumeration enu = allInstances.enumerateInstances();
			while( enu.hasMoreElements() ) {
				instance = (Instance) enu.nextElement();
				if( !instance.isMissing( m_attIndex ) ) {
					tempValue = instance.value( m_attIndex );
					if( Utils.gr( tempValue, newSplitPoint ) &&
							Utils.smOrEq( tempValue, m_splitPoint ) ) {
						newSplitPoint = tempValue;
					}
				}
			}
			m_splitPoint = newSplitPoint;
		}
	}

	/**
	 * Returns the minsAndMaxs of the index.th subset.
	 */
	public final double[][] minsAndMaxs( Instances data, double[][] minsAndMaxs,
			int index ) {

		double[][] newMinsAndMaxs = new double[data.numAttributes()][2];

		for( int i = 0; i < data.numAttributes(); i++ ) {
			newMinsAndMaxs[i][0] = minsAndMaxs[i][0];
			newMinsAndMaxs[i][1] = minsAndMaxs[i][1];
			if( i == m_attIndex ) {
				if( data.attribute( m_attIndex ).isNominal() ) {
					newMinsAndMaxs[m_attIndex][1] = 1;
				} else {
					newMinsAndMaxs[m_attIndex][1 - index] = m_splitPoint;
				}
			}
		}

		return newMinsAndMaxs;
	}

	/**
	 * Sets distribution associated with model.
	 */
	public void resetDistribution( Instances data ) throws Exception {

		Instances insts = new Instances( data, data.numInstances() );
		for( int i = 0; i < data.numInstances(); i++ ) {
			if( whichSubset( data.instance( i ) ) > -1 ) {
				insts.add( data.instance( i ) );
			}
		}
		Distribution newD = new Distribution( insts, this );
		newD.addInstWithUnknown( data, m_attIndex );
		m_distribution = newD;
	}

	/**
	 * Returns weights if instance is assigned to more than one subset.
	 * Returns null if instance is only assigned to one subset.
	 */
	public final double[] weights( Instance instance ) {

		double[] weights;
		int i;

		if( instance.isMissing( m_attIndex ) ) {
			weights = new double[m_numSubsets];
			for( i = 0; i < m_numSubsets; i++ ) {
				weights[i] = m_distribution.perBag( i ) / m_distribution.total();
			}
			return weights;
		} else {
			return null;
		}
	}

	/**
	 * Returns index of subset instance is assigned to.
	 * Returns -1 if instance is assigned to more than one subset.
	 *
	 * @exception Exception if something goes wrong
	 */
	public final int whichSubset( Instance instance )
			throws Exception {

		if( instance.isMissing( m_attIndex ) ) {
			return -1;
		} else {
			if( instance.attribute( m_attIndex ).isNominal() ) {
				return (int) instance.value( m_attIndex );
			} else if( Utils.smOrEq( instance.value( m_attIndex ), m_splitPoint ) ) {
				return 0;
			} else {
				return 1;
			}
		}
	}

	/**
	 * Returns the revision string.
	 * 
	 * @return		the revision
	 */
	public String getRevision() {
		return RevisionUtils.extract( "$Revision: 1.00 $" );
	}
}
