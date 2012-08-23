package weka.classifiers.meta;

import java.util.Enumeration;
import java.util.Vector;
import weka.classifiers.RandomizableMultipleClassifiersCombiner;
import weka.core.Instance;
import weka.core.Instances;
import weka.core.Option;
import weka.core.RevisionUtils;
import weka.core.TechnicalInformation;
import weka.core.TechnicalInformationHandler;
import weka.core.Utils;

/**
 *
 * @author rlichten
 */
public class WeightedVote extends RandomizableMultipleClassifiersCombiner implements TechnicalInformationHandler {

	protected double[] m_Weights;

	public void setWeights( double[] weights ) {
		double min = weights[Utils.minIndex(weights)];
		if( min < 0 ) {
			for( int i = 0; i < weights.length; i++ ) {
				weights[i] += -min;
			}
		}
		double sum = Utils.sum( weights );
		if( sum <= 0 ) {
			for( int i = 0; i < weights.length; i++ ) {
				weights[i] = 1.0 / (double)weights.length;
			}
		}

		Utils.normalize( weights );
		m_Weights = weights;
	}

	public double[] getWeights() {
		return m_Weights;
	}

	/**
	 * Returns an enumeration describing the available options.
	 *
	 * @return an enumeration of all the available options.
	 */
	public Enumeration listOptions() {
		Vector result = new Vector();
		Enumeration optionEnum = super.listOptions();
		while( optionEnum.hasMoreElements() ) {
			result.addElement( optionEnum.nextElement() );
		}
		result.addElement( new Option(
				"\tThe weights to use (default: even weighting)\n",
				"w", 1, "-w <comma-separated-weights>" ) );

		return result.elements();
	}

	public void setOptions( String[] options ) throws Exception {
		String weightStr = Utils.getOption( 'w', options );
		if( weightStr.length() != 0 ) {
			String[] weightStrArray = weightStr.split( "," );
			double[] weightsArray = new double[weightStrArray.length];
			for( int i = 0; i < weightStrArray.length; i++ ) {
				weightsArray[i] = new Double( weightStrArray[i] ).doubleValue();
			}
			setWeights( weightsArray );
		}
	}

	public String[] getOptions() {
		String[] options = new String[4];
		int current = 0;

		String weightStr = "";
		for( int i = 0; i < m_Weights.length; i++ ) {
			weightStr += m_Weights[i] + ",";
		}
		weightStr.replace( ",$", "" );

		options[current++] = "-w";
		options[current++] = "" + weightStr;

		while( current < options.length ) {
			options[current++] = "";
		}
		return options;
	}

	/**
	 * Classifies the given test instance.
	 *
	 * @param instance the instance to be classified
	 * @return the predicted most likely class for the instance or 
	 * Instance.missingValue() if no prediction is made
	 * @throws Exception if an error occurred during the prediction
	 */
	public double classifyInstance( Instance instance ) throws Exception {
		double result;

		double[] dist = distributionForInstance( instance );
		if( instance.classAttribute().isNominal() ) {
			int index = Utils.maxIndex( dist );
			if( dist[index] == 0 ) {
				result = Instance.missingValue();
			} else {
				result = index;
			}
		} else if( instance.classAttribute().isNumeric() ) {
			result = dist[0];
		} else {
			result = Instance.missingValue();
		}
		return result;
	}

	/**
	 * Classifies a given instance using the selected combination rule.
	 *
	 * @param instance the instance to be classified
	 * @return the distribution
	 * @throws Exception if instance could not be classified
	 * successfully
	 */
	public double[] distributionForInstance( Instance instance ) throws Exception {
		double[] result = new double[instance.numClasses()];

		for( int i = 0; i < m_Classifiers.length; i++ ) {
			double[] dist = getClassifier( i ).distributionForInstance( instance );
			for( int j = 0; j < result.length; j++ ) {
				result[j] += m_Weights[i] * dist[j];
			}

		}

		if( !instance.classAttribute().isNumeric() && ( Utils.sum( result ) > 0 ) ) {
			Utils.normalize( result );
		}

		return result;
	}

	/**
	 * This method selects a classifier from the set of classifiers
	 * by minimising error on the training data.
	 *
	 * @param data the training data to be used for generating the
	 * classifier.
	 * @throws Exception if the classifier could not be built successfully
	 */
	public void buildClassifier( Instances data ) throws Exception {

		getCapabilities().testWithFail( data );

		Instances newData = new Instances( data );
		newData.deleteWithMissingClass();

		for( int i = 0; i < m_Classifiers.length; i++ ) {
			getClassifier( i ).buildClassifier( newData );
		}
	}

	public String getRevision() {
		return RevisionUtils.extract( "$Revision: 1.0$" );
	}

	public TechnicalInformation getTechnicalInformation() {
		throw new UnsupportedOperationException( "Not supported yet." );
	}

	/**
	 * Main method for testing this class.
	 *
	 * @param argv should contain the following arguments:
	 * -t training file [-T test file] [-c class index]
	 */
	public static void main( String[] argv ) {
		runClassifier( new WeightedVote(), argv );
	}
}
