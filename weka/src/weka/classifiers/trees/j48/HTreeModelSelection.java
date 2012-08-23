package weka.classifiers.trees.j48;

import weka.core.Attribute;
import weka.core.Instances;
import weka.core.RevisionUtils;
import weka.core.Utils;

import java.util.Enumeration;

/**
 * Class for selecting a Hellinger tree split for a given dataset.
 *
 * @author Ryan Lichtenwalter (rlichten@cse.nd.edu)
 * @version $Revision: 1.00 $
 */
public class HTreeModelSelection extends ModelSelection {

	/** Minimum number of objects in interval. */
	private int m_minNoObj;
	/** All the training data */
	private Instances m_allData;

	/**
	 * Initializes the split selection method with the given parameters.
	 *
	 * @param minNoObj minimum number of instances that have to occur in at least two
	 * subsets induced by split
	 * @param allData FULL training dataset (necessary for
	 * selection of split points).
	 */
	public HTreeModelSelection( int minNoObj, Instances allData ) {
		m_minNoObj = minNoObj;
		m_allData = allData;
	}

	/**
	 * Sets reference to training data to null.
	 */
	public void cleanup() {
		m_allData = null;
	}

	/**
	 * Selects Hellinger tree split for the given dataset.
	 */
	public final ClassifierSplitModel selectModel( Instances data ) {
		try {
			// Check if all Instances belong to one class or if not enough Instances to split.
			Distribution checkDistribution = new Distribution( data );
			NoSplit noSplitModel = new NoSplit( checkDistribution );
			if( Utils.sm( checkDistribution.total(), 2 * m_minNoObj ) || Utils.eq( checkDistribution.total(), checkDistribution.perClass( checkDistribution.maxClass() ) ) ) {
				return noSplitModel;
			}
			
			// Check if all attributes are nominal and have a lot of values.
			boolean multiVal = true;
			if( m_allData != null ) {
				Enumeration attrEnum = data.enumerateAttributes();
				while( attrEnum.hasMoreElements() ) {
					Attribute attribute = (Attribute) attrEnum.nextElement();
					if( ( attribute.isNumeric() ) || ( Utils.sm( (double) attribute.numValues(), ( 0.3 * (double) m_allData.numInstances() ) ) ) ) {
						multiVal = false;
						break;
					}
				}
			}

			HTreeSplit[] currentModel = new HTreeSplit[data.numAttributes()];
			double sumOfWeights = data.sumOfWeights();

			// For each attribute except class attribute.
			double averageDistance = 0;
			int validModels = 0;
			for( int i = 0; i < data.numAttributes(); i++ ) {
				if( i != data.classIndex() ) {

					// Get models for current attribute.
					currentModel[i] = new HTreeSplit( i, m_minNoObj, sumOfWeights );
					currentModel[i].buildClassifier( data );

					// Check if useful split for current attribute exists and check for enumerated attributes with a lot of values.
					if( currentModel[i].checkModel() ) {
						if( m_allData != null ) {
							if( ( data.attribute( i ).isNumeric() ) || ( multiVal || Utils.sm( (double) data.attribute( i ).numValues(),( 0.3 * (double) m_allData.numInstances() ) ) ) ) {
								averageDistance += currentModel[i].hellingerDistance();
								validModels++;
							}
						} else {
							averageDistance += currentModel[i].hellingerDistance();
							validModels++;
						}
					}
				} else {
					currentModel[i] = null;
				}
			}

			// Check if any useful split was found.
			if( validModels == 0 ) {
				return noSplitModel;
			}
			averageDistance = averageDistance / (double) validModels;

			// Find "best" attribute to split on.
			HTreeSplit bestModel = null;
			double bestDistance = Double.NEGATIVE_INFINITY;
			for( int i = 0; i < data.numAttributes(); i++ ) {
				if( i != data.classIndex() && currentModel[i].checkModel() ) {
					double currentDistance = currentModel[i].hellingerDistance();
					if( currentDistance > bestDistance ) {
						bestDistance = currentDistance;
						bestModel = currentModel[i];
					}
				}
			}
			
			// Use 1E-3 here to get a closer approximation to the original implementation.
			if( bestModel == null ) {
				return noSplitModel;
			} else if( bestDistance < averageDistance - 1E-3 ) {
				return noSplitModel;
			}

			// Add all Instances with unknown values for the corresponding attribute to the distribution for the model, so that
			// the complete distribution is stored with the model. 
			bestModel.distribution().addInstWithUnknown( data, bestModel.attIndex() );

			// Set the split point analogue if attribute numeric.
			if( m_allData != null ) {
				bestModel.setSplitPoint( m_allData );
			}
			return bestModel;
		} catch( Exception e ) {
			e.printStackTrace();
		}
		return null;
	}

	/**
	 * Selects Hellinger tree split for the given dataset.
	 */
	public final ClassifierSplitModel selectModel( Instances train, Instances test ) {
		return selectModel( train );
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
