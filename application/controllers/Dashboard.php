<?php
defined('BASEPATH') or exit('No direct script access allowed');

class Dashboard extends CI_Controller
{

	public function __construct()
	{
		parent::__construct();
		
		$this->load->model('M_Data', 'data');
	}

	public function index()
	{
		$data['title'] = 'Dashboard';
		$data['page'] = 'backend/dashboard';
		$data['data'] = $this->data->getAll();
		$data['akses'] = $this->data->getAkses();

		$this->load->view('backend/index', $data);
	}

	public function editAkses()
	{
		$this->data->editAkses();
		$this->session->set_flashdata('flash-success', 'Akses berhasil diedit');
		redirect('Dashboard');
	}

	public function hapusUser($id)
	{
		$this->data->hapus($id);
		$this->session->set_flashdata('flash-sukses', 'Data berhasil dihapus');
		redirect('Dashboard');
	}

	public function editUser()
	{
		$this->data->editUser();
		$this->session->set_flashdata('flash-success', 'User berhasil diedit');
		redirect('Dashboard');
	}
}

/* End of file Dashboard.php */
/* Location: ./application/controllers/Dashboard.php */