import pytest
from flexauto import AthenaAutocallable, ABM, GBM, AthenaResult  # Update with the correct module name

experiment_configuration = {
    "drift": 0.05,
    "volatility": 0.06,
    "spot_price": 1.0,
    "maturity": 5.0,  # years
    "step_size": 0.01,
    "number_of_steps": 1000
}

athena_configuration = {
    "coupon_barrier": 1.1,
    "autocall_barrier": 1.1,
    "autocall_value": 1.0,
    "exit_barrier": 1.2,
    "kill_barrier": 0.8,
    "maturity": experiment_configuration["maturity"],
    "observation_dates": [1.0, 2.0, 3.0],
    "coupon_value": 0.05,
    "kill_value": 0.8,
    "inception_spot": 1.0,
}

# Fixture to create an instance of AthenaAutocallable
@pytest.fixture
def athena():
    return AthenaAutocallable(*athena_configuration.values())

def test_price_abm(athena):
    # Test that price_abm returns the expected result
    abm = ABM(*experiment_configuration.values())
    abm.generate_stock_price()
    result = athena.price_abm(abm)
    expected_result = None  # Replace with the expected result
    assert result == expected_result

def test_price_gbm(athena):
    # Test that price_gbm returns the expected result
    gbm = GBM(*experiment_configuration.values())
    gbm.generate_stock_price()
    result = athena.price_gbm(gbm)
    expected_result = None  # Replace with the expected result
    assert result == expected_result

def test_preliminary_checks(athena):
    # Test that preliminary_checks runs without errors
    try:
        athena.preliminary_checks()
    except Exception as e:
        pytest.fail(f"preliminary_checks raised an exception: {e}")

def test_repr(athena):
    result = repr(athena)
    expected_result = "AthenaAutocallable(exit=" + str(athena.exit_barrier) + \
																	", autocall=" + str(athena.autocall_barrier) + \
																	", coupon=" + str(athena.coupon_barrier) + \
																	", kill=" + str(athena.kill_barrier) +")"
    assert result == expected_result


@pytest.fixture
def sample_athena_result():
    athena = AthenaAutocallable(*athena_configuration.values())
    gbm = GBM(*experiment_configuration.values())
    return athena.price_gbm(gbm)

def sample_athena_result_abm():
    athena = AthenaAutocallable(*athena_configuration.values())
    abm = ABM(*experiment_configuration.values())
    return athena.price_abm(abm)

def test_generate_json_dump(sample_athena_result):
    json_dump = sample_athena_result.generate_json_dump()
    assert isinstance(json_dump, str)


def test_get_autocall_barrier(sample_athena_result):
    assert sample_athena_result.getAutocallBarrier() == 1.1

def test_get_exit_barrier(sample_athena_result):
    assert sample_athena_result.getExitBarrier() == 1.2

def test_get_kill_barrier(sample_athena_result):
    assert sample_athena_result.getKillBarrier() == 0.8

# Not sure how to implement
# def test_get_underlying_path(sample_athena_result):
#     assert sample_athena_result.getUnderlyingPath() == 

def test_get_obs_dates(sample_athena_result):
    assert sample_athena_result.getObsDates() == [1.0, 2.0, 3.0]

def test_get_inception_spot(sample_athena_result):
    assert sample_athena_result.getInceptionSpot() == 1.0

def test_get_maturity(sample_athena_result):
    assert sample_athena_result.getMaturity() == 5.0

# Not sure how to implement
def test_get_termination_status(sample_athena_result):
    assert sample_athena_result.getTerminationStatus() == ""  
    
def test_repr(sample_athena_result):
    expected_repr = (
        "AthenaResult(status=\"" + sample_athena_result.getTerminationStatus() + "\", " + "price=" + str(sample_athena_result.getPrice()) + ")"
    )  # Adjust based on the actual termination status and format
    assert repr(sample_athena_result) == expected_repr